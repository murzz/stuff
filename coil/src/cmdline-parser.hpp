#pragma once

#include <cctype>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/log/trivial.hpp>

#include <htmlcxx/html/ParserDom.h>
#include "board.hpp"
#include "curl.hpp"
#include "env.hpp"

namespace cmdline
{
/// forward declaration
template<typename Handler>
void parse(boost::asio::io_service & io_service, Handler handler, int argc, char ** argv,
   boost::shared_ptr<std::stringstream> config_data);

namespace internal
{

/// helper
template<typename Handler>
struct templated_functor
{
//   typedef boost::function<
//      void(boost::asio::io_service & io_service, Handler handler, int argc, char ** argv,
//         boost::shared_ptr<std::stringstream> config_data)> parse_type;

   typedef boost::function<
      void(boost::asio::io_service & io_service, Handler handler, const std::string & html)> html_handler_type;
};

struct options
{
   std::size_t level_;
   std::size_t x_;
   std::size_t y_;
   std::string board_;

//   std::string url_;
//   std::string name_;
//   std::string pass_;
   options() :
      level_(0), x_(0), y_(0)
   {
   }
};

void get_board_string(std::string& html_board, const std::string & html)
{
   std::string html_board2;

   // we have 2 places with FlashVars defined, parse it both and compare

   htmlcxx::HTML::ParserDom parser;
   tree<htmlcxx::HTML::Node> dom = parser.parseTree(html);
   tree<htmlcxx::HTML::Node>::iterator it = dom.begin();
   tree<htmlcxx::HTML::Node>::iterator end = dom.end();
   for (; it != end; ++it)
   {
      if (it->tagName() == "param")
      {
         it->parseAttributes();
         if ("FlashVars" == it->attribute("name").second)
         {
            html_board = it->attribute("value").second;
         }
      }

      if (it->tagName() == "embed")
      {
         it->parseAttributes();
         html_board2 = it->attribute("flashvars").second;
//         auto m = it->attributes();
//         for (const auto & p : m)
//         {
//            std::cout << p.first << " " << p.second << std::endl;
//         }
      }

      if (!html_board.empty() && !html_board2.empty())
      {
         break; // early
      }
   }

   if (html_board != html_board2)
   {
      std::string err = "Error parsing html_board: ";
      err += "'" + html_board + "' != '" + html_board2 + "'";
      err += ", html = '" + html + "'";
      throw std::invalid_argument(err.c_str());
   }

   // find level
   const std::string level_prefix = "Level: ";
   const std::string::size_type start_idx = html.find(level_prefix);
   if (std::string::npos == start_idx)
   {
      std::string err = "Failed to find level";
      err += ", html = '" + html + "'";
      throw std::invalid_argument(err.c_str());
   }

   //
   std::string level_string;
   // loop until non numeric char found
   for (size_t idx = start_idx + level_prefix.size(); idx < html.size(); ++idx)
   {
      std::string::value_type c = html.at(idx);
      if (std::isdigit(c))
      {
         level_string += c;
      }
      else
      {
         break;
      }
   }

   html_board += "&level=" + level_string;
}

/// handler is called whenever there is a need to transform board in html form to config form
template<typename Handler>
void html_handler(boost::asio::io_service & io_service, Handler handler, const std::string & html)
{
   // parse out board from html: <param name="FlashVars" value="x=5&y=3&board=......X......X." />
   std::string html_board;

   get_board_string(html_board, html);

   if (html_board.empty())
   {
      std::string err = "Board not found in html";
//#ifndef NDEBUG
      err += ": " + html;
//#endif
      throw std::invalid_argument(err.c_str());
   }

   // split to x, y and board
   // std::vector<std::string> strs;
//   boost::split(strs, html_board, boost::is_any_of("&"));
   //std::copy(strs.begin(), strs.end(), std::ostream_iterator<std::string>(std::cout, " "));

   // make it config-style
   std::replace(html_board.begin(), html_board.end(), '&', '\n');

   boost::shared_ptr<std::stringstream> config_data(
      boost::make_shared<std::stringstream>(html_board.c_str()));

   // parse config data
   io_service.post(
      boost::bind(parse<Handler>, boost::ref(io_service), handler, 0, nullptr, config_data));
}

void save(const internal::options & options)
{
   BOOST_LOG_TRIVIAL(info) << "Saving level #" << options.level_;

   std::stringstream name;
   name << "level_" << options.level_;
//      << "_" << boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());

   std::stringstream content;
   content << "level = " << options.level_ << std::endl;
   content << "x = " << options.x_ << std::endl;
   content << "y = " << options.y_ << std::endl;
   content << "board = " << options.board_ << std::endl;

   std::ofstream file(name.str().c_str());
   file << content.rdbuf();
}

/// perform actions defined by command line
template<typename Handler>
void perform(boost::asio::io_service & io_service, Handler handler, options options)
{
   if (options.x_ && options.y_ && !options.board_.empty())
   {
      // board available

      // save level
      io_service.post(boost::bind(save, options));

      // create board
      coil::board board(options.x_, options.y_, options.board_, options.level_);

      // solve it
      BOOST_LOG_TRIVIAL(info)<< "Start solving level";
      board.started_solving_ = boost::posix_time::second_clock::local_time();
      io_service.post(boost::bind(handler.template get<0>(), board));
   }
   else if (!env::get().url_.empty() && !env::get().name_.empty() && !env::get().pass_.empty())
   {
      // download board, then it will be parsed during general flow
      BOOST_LOG_TRIVIAL(info) << "Downloading level";

      const std::string url = env::get().url_ + "?name=" + env::get().name_ + "&password="
         + env::get().pass_;

//      typedef typename helper<Handler>::parse_type parse_type;
//      parse_type parse_functor = boost::bind(::cmdline::parse<Handler>, boost::ref(io_service),
//         handler, 0, nullptr, _5);

      typedef typename templated_functor<Handler>::html_handler_type html_handler_type;
      html_handler_type html_handler_functor = boost::bind(html_handler<Handler>,
         boost::ref(io_service),
         handler, _3);

      io_service.post(
         boost::bind(curl::download<boost::tuple<html_handler_type, Handler> >,
            boost::ref(io_service),
            boost::make_tuple(html_handler_functor, handler), url));
   }
   else
   {
      // nothing to do
      throw std::invalid_argument(
         "Either Board options or Account options should be defined. Try --help option.");
   }
}
} //namespace internal

/// parse options from command line, config file, env vars
template<typename Handler>
void parse(boost::asio::io_service & io_service, Handler handler, int argc, char ** argv,
   boost::shared_ptr<std::stringstream> config_data)
{
   namespace po = boost::program_options;
   internal::options options;

   // prefix for env vars
   const std::string prefix = "hacker_org_";
   const std::string url_template = "http://www.hacker.org/coil/index.php";

   // define options
   std::string config_file_name;
   po::options_description general_options("General options");
   general_options.add_options()
   ("help,h", "produce help message")
   ("version,v", "print program version")
   ("file,f", po::value<std::string>(&config_file_name),
      "read board configuration from file");

   po::options_description board_options("Board options");
   board_options.add_options()
   ("level,l", po::value<std::size_t>(&options.level_), "set board level")
   ("x", po::value<std::size_t>(&options.x_), "set board width") // name forced to be able to read from file
   ("y", po::value<std::size_t>(&options.y_), "set board height")
   ("board,b", po::value<std::string>(&options.board_),
      "defines board itself as a string of 'X's and '.'s (set of squares)");

   po::options_description account_options("Account options");
   account_options.add_options()
   ("url,u",
      po::value<std::string>(&env::get().url_)->default_value(url_template),
      "URL template to be used for GET request")
   ("name,n", po::value<std::string>(&env::get().name_),
      ("registered user name at hacker.org (could be env var prefixed with '" + prefix + "')").c_str())
   ("password,p", po::value<std::string>(&env::get().pass_),
      ("password (could be env var prefixed with '" + prefix + "')").c_str());

   // put options together
   po::options_description cmdline_options;
   cmdline_options.add(general_options).add(board_options).add(account_options);

   po::variables_map vm;

   // get options defined in command line
   if (argc)
   {
      po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
   }

   if (/*vm.empty() ||*/vm.count("help"))
   {
      std::cout << general_options << board_options << account_options << std::endl;
      return;
   }

   if (vm.count("version"))
   {
      //TODO: implement version
      std::cout << "not implemented" << std::endl;
      return;
   }

   // get options defined in stream file
   if (config_data.get())
   {
      //TODO do we need notify here?

      po::notify(vm);
      po::store(po::parse_config_file(*config_data, board_options), vm);
   }

   if (vm.count("file"))
   {
      // get options defined in config file
      po::notify(vm);
      po::store(po::parse_config_file<std::string::value_type>
         (config_file_name.c_str(), board_options), vm);
   }

   // get options defined as env vars, these should be prefixed with prefix
   po::store(po::parse_environment(account_options, prefix), vm);

   // final notify
   po::notify(vm);

   // next action
   io_service.post(
      boost::bind(internal::perform<Handler>, boost::ref(io_service), handler, options));
}

} //namespace cmdline
