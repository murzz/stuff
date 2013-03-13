#pragma once

#include <stdexcept>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>
#include "board.hpp"

//TODO: make it boost error code
class board_not_parsed: public std::exception
{

};

void parse(const int& argc, const char * const * argv, size_t& x, size_t& y,
        std::string& squares)
{
    namespace po = boost::program_options;
//    std::cout<<"argc="<<argc<<" argv=";
//
//    for(size_t idx = 0; idx < argc; ++idx)
//    {
//        std::cout<<argv[idx] << ' ';
//    }

//    po::options_description cmdline_options;
//    cmdline_options.add(generic).add(config).add(hidden);
//
//    po::options_description config_file_options;
//    config_file_options.add(config).add(hidden);
//
//    po::options_description visible("Allowed options");
//    visible.add(generic).add(config);

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
    ("x", po::value<size_t>(&x), "set board width")
    ("y", po::value<size_t>(&y), "set board height")
    ("board", po::value<std::string>(&squares),
            "defines board itself as a vector of 'X's and '.'s");

    // put options together
    po::options_description cmdline_options;
    cmdline_options.add(general_options).add(board_options);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, cmdline_options), vm);

    if (vm.empty() || vm.count("help"))
    {
        std::cout << general_options << board_options << std::endl;
        throw board_not_parsed();
    }

    if (vm.count("version"))
    {
        //TODO: implement version
        std::cout << "version not implemented" << std::endl;
        throw board_not_parsed();
    }

    // read from file
    if (vm.count("file"))
    {

        po::notify(vm);
        vm.clear();
        std::cout << "reading board from file '" << config_file_name << "'" << std::endl;

        po::options_description config_file_options;
        config_file_options.add(board_options);
        po::store(po::parse_config_file<std::string::value_type>
                (config_file_name.c_str(), config_file_options), vm);
    }

    po::notify(vm);

    // board should be defined here via cmd line or config file
    if (!(vm.count("x") && vm.count("y") && vm.count("board")))
    {
        throw board_not_parsed();
    }
}

