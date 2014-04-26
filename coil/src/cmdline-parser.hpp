#pragma once

#include <boost/program_options.hpp>
//#include <boost/system/error_code.hpp>
#include "board.hpp"

template<typename ErrorHandler>
//void parse(const int& argc, const char * const * argv, ErrorHandler handler)
void parse(int argc, char** argv, ErrorHandler handler)
{
   boost::system::error_code ec;
   handler.template get<0>()(ec);
   //, size_t& x, size_t& y,   std::string& squares
//    namespace po = boost::program_options;
//
//    // define options
//    std::string config_file_name;
//    po::options_description general_options("General options");
//    general_options.add_options()
//    ("help,h", "produce help message")
//    ("version,v", "print program version")
//    ("file,f", po::value<std::string>(&config_file_name),
//            "read board configuration from file");
//
//    po::options_description board_options("Board options");
//    board_options.add_options()
//    ("x", po::value<size_t>(&x), "set board width")
//    ("y", po::value<size_t>(&y), "set board height")
//    ("board", po::value<std::string>(&squares),
//            "defines board itself as a string of 'X's and '.'s (set of squares)");
//
//    // put options together
//    po::options_description cmdline_options;
//    cmdline_options.add(general_options).add(board_options);
//
//    po::variables_map vm;
//    po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
//
//    if (vm.empty() || vm.count("help"))
//    {
//        std::cout << general_options << board_options << std::endl;
//        throw board_not_parsed();
//    }
//
//    if (vm.count("version"))
//    {
//        //TODO: implement version
//        std::cout << "version not implemented" << std::endl;
//        throw board_not_parsed();
//    }
//
//    // read from file
//    if (vm.count("file"))
//    {
//
//        po::notify(vm);
//        std::cout << "reading board from file '" << config_file_name << "'" << std::endl;
//
//        po::options_description config_file_options;
//        config_file_options.add(board_options);
//        po::store(po::parse_config_file<std::string::value_type>
//                (config_file_name.c_str(), config_file_options), vm);
//    }
//
//    po::notify(vm);
//
//    // board should be defined here via cmd line or config file
//    if (!(vm.count("x") && vm.count("y") && vm.count("board")))
//    {
//        throw board_not_parsed();
//    }
}

