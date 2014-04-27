#pragma once

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>
#include "board.hpp"

/// perform actions defined by command line
template<typename ErrorHandler>
void perform(boost::asio::io_service & ios, const boost::program_options::variables_map & vm,
   ErrorHandler handler)
{
//   for (const auto & p : vm)
//   {
//      std::cout << p.first.c_str();
//      std::cout << "=";
//      std::cout << boost::any_cast<std::string>(p.second.value());
//      std::cout << std::endl;
//   }

   std::cout << vm["name"].as<std::string>() << std::endl;
   return;
   if (vm.count("x") && vm.count("y") && vm.count("board"))
   {
      // solve board
      //ios.post();
      //throw board_not_parsed();
   }
   else
   {
      // download board

//      const std::string url = std::string("http://www.hacker.org/coil/index.php")
//         + "?name=" + vm["name"].value() + "&password=" + vm["pass"].value();

      // coil::board board(ios, url);
   }
}

/// parse command line
template<typename ErrorHandler>
void parse(boost::asio::io_service & ios, int argc, char ** argv, ErrorHandler handler)
{
   namespace po = boost::program_options;

   // prefix for env vars
   const std::string prefix = "hacker_org_";
   const std::string base_url = "http://www.hacker.org/coil/index.php";

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
   ("x", "set board width") // name forced to be able to read from file
   ("y", "set board height")
   ("board,b", "defines board itself as a string of 'X's and '.'s (set of squares)");

   po::options_description account_options("Account options");
   account_options.add_options()
      ("url,u",
         po::value<std::string>()->default_value(base_url), "base URL to be used for GET and POST requests"),
      ("name,n",
         ("registered user name at hacker.org (could be env var prefixed with '" + prefix + "')").c_str()),
      ("password,p",
         std::string("password (could be env var prefixed with '" + prefix + "')").c_str());

   // put options together
   po::options_description cmdline_options;
   cmdline_options.add(general_options).add(board_options).add(account_options);

   po::variables_map vm;

   // get options defined in command line
   po::store(po::parse_command_line(argc, argv, cmdline_options), vm);

   if (/*vm.empty() ||*/vm.count("help"))
   {
      std::cout << general_options << board_options << account_options << std::endl;

      // return with no error
      boost::system::error_code ec;
      handler.template get<0>()(ec);
      return;
   }

   if (vm.count("version"))
   {
      //TODO: implement version
      std::cout << "not implemented" << std::endl;

      // return with no error
      boost::system::error_code ec;
      handler.template get<0>()(ec);
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
   ios.post(boost::bind(perform<ErrorHandler>, boost::ref(ios), vm, handler));
}

