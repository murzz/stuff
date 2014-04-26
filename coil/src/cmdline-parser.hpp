#pragma once

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>
#include "board.hpp"

template<typename ErrorHandler>
void parse(boost::asio::io_service & ios, int argc, char ** argv, ErrorHandler handler)
{
   namespace po = boost::program_options;

   // define options
   std::string config_file_name;
   po::options_description general_options("General options");
   general_options.add_options()
   ("help,h", "produce help message")
   ("version,v", "print program version")
   ("file,f", po::value<std::string>(&config_file_name),
      "read board configuration from file");

   po::options_description board_options("Board options");
   size_t x = 0;
   size_t y = 0;
   std::string squares;
   board_options.add_options()
   ("x", po::value<size_t>(&x), "set board width") // name forced to be able to read from file
   ("y", po::value<size_t>(&y), "set board height")
   ("board,b", po::value<std::string>(&squares),
      "defines board itself as a string of 'X's and '.'s (set of squares)");

   po::options_description account_options("Account options");
   std::string name;
   std::string pass;
   account_options.add_options()
   ("name,n", po::value<std::string>(&name), "registered user name at hacker.org")
   ("password,p", po::value<std::string>(&pass), "password");

   // put options together
   po::options_description cmdline_options;
   cmdline_options.add(general_options).add(board_options).add(account_options);

   po::variables_map vm;
   po::store(po::parse_command_line(argc, argv, cmdline_options), vm);

   if (/*vm.empty() ||*/ vm.count("help"))
   {
      std::cout << general_options << board_options << account_options << std::endl;

      boost::system::error_code ec;
      handler.template get<0>()(ec);
      return;
   }

   if (vm.count("version"))
   {
      //TODO: implement version
      std::cout << "not implemented" << std::endl;

      boost::system::error_code ec;
      handler.template get<0>()(ec);
   }

   // read from file
   if (vm.count("file"))
   {
      po::notify(vm);
      //    std::cout << "reading options from configuration file '" << config_file_name << "'"
      //       << std::endl;

      po::options_description config_file_options;
      config_file_options.add(board_options);
      po::store(po::parse_config_file<std::string::value_type>
         (config_file_name.c_str(), config_file_options), vm);
   }

   po::notify(vm);

   if (vm.count("x") && vm.count("y") && vm.count("board"))
   {
      // solve board
      //ios.post();
      //throw board_not_parsed();
   }
   else
   {
      // download board

      // if name/pass not defined, resolve it from env
      if (!vm.count("name"))
      {
         name = std::getenv("name");
      }

      if (!vm.count("pass"))
      {
         pass = std::getenv("pass");
      }

      const std::string url = std::string("http://www.hacker.org/coil/index.php")
         + "?name=" + name + "&password=" + pass;

      coil::board board(ios, url);
   }
}

