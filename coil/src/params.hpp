#pragma once

#include <string>
#include <vector>

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

void parse(board& board, int argc, char** argv)
{
    namespace po = boost::program_options;
    std::string board_str;
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "produce help message")
    ("width,x", po::value<size_t>(&board.x_)->required(), "set board width")
    ("height,y", po::value<size_t>(&board.y_)->required(), "set board height")

    ("board,board",
            po::value<std::string>(&board_str)->required(),
            "defines board itself")

//    ("board,board",
//            po::value<board::type>(&board.board_)->multitoken()->required(),
//            "defines board itself")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    for (const std::string::value_type& square_str : board_str)
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
                    boost::program_options::validation_error::invalid_option_value,"board",board_str);
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

