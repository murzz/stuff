#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include <boost/program_options.hpp>

struct board
{
    size_t x_;
    size_t y_;

    typedef enum square_type_tag
    {
        empty = '.',
        wall = 'X'
    } square_type;
    typedef std::vector<square_type> type;
    type board_;
    std::string board_str_;
};

//std::istream& operator>>(std::istream& in, board::square_type& square)
//{
//    std::string token;
//    in >> token;
//    std::cout << token << std::endl;
//    for (const std::string::value_type& square_token : token)
//    {
//        board::square_type st = static_cast<board::square_type>(square_token);
//
//        switch (st)
//        {
//        case board::empty:
//        case board::wall:
//            square = st;
//            break;
//        default:
//            throw boost::program_options::validation_error(
//                    boost::program_options::validation_error::invalid_option_value);
//        }
//    }
//
//    return in;
//}

class exit_with_success: public std::exception
{
//public:
    //help_printed
};

void parse(board& board, int argc, char** argv)
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
    board_options.add_options()
    ("x,width", po::value<size_t>(&board.x_)->required(), "set board width")
    ("y,height", po::value<size_t>(&board.y_)->required(), "set board height")
    ("board,b", po::value<std::string>(&board.board_str_)->required(),
            "defines board itself as a vector of 'X's and '.'s");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, general_options), vm);

    if (vm.empty() || vm.count("help"))
    {
        std::cout << general_options << board_options << std::endl;
        throw exit_with_success();
    }

    if (vm.count("version"))
    {
        //TODO: implement version
        std::cout << "version not implemented" << std::endl;
        throw exit_with_success();
    }

    // read from file
    if (vm.count("file"))
    {
        po::notify(vm);
        std::cout << "reading board from file '" << config_file_name << "'"
                << std::endl;
        po::store(po::parse_config_file<std::string::value_type>
                (config_file_name.c_str(), board_options), vm);
    }

    po::notify(vm);

    for (const std::string::value_type& square_str : board.board_str_)
    {
        board::square_type square = static_cast<board::square_type>(square_str);
        switch (square)
        {
        case board::empty:
            case board::wall:
            board.board_.push_back(square);
            break;
        default:
            throw boost::program_options::validation_error(
                    boost::program_options::validation_error::invalid_option_value,
                    "board", board.board_str_);
        }
    }

//        if (vm.count("help"))
//        {
//            std::cout << desc << "\n";
//            //return 1;
//        }
//
//        if (vm.count("compression"))
//        {
//            std::cout << "Compression level was set to "
//                    << vm["compression"].as<int>() << ".\n";
//        }
//        else
//        {
//            std::cout << "Compression level was not set.\n";
//        }
//
//        // Declare a group of options that will be
//        // allowed only on command line
//        po::options_description generic("Generic options");
//        generic.add_options()("version,v", "print version string")("help",
//                "produce help message");
//
//        // Declare a group of options that will be
//        // allowed both on command line and in
//        // config file
//        int opt;
//        po::options_description config("Configuration");
//        config.add_options()("optimization",
//                po::value<int>(&opt)->default_value(10), "optimization level")(
//                "include-path,I",
//                po::value<std::vector<std::string> >()->composing(),
//                "include path");
//
//        // Hidden options, will be allowed both on command line and
//        // in config file, but will not be shown to the user.
//        po::options_description hidden("Hidden options");
//        hidden.add_options()("input-file",
//                po::value<std::vector<std::string> >(), "input file");
}

