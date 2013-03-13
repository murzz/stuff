#pragma once

#include <array>

//#define BOOST_TEST_MODULE parser tests
#include <boost/test/unit_test.hpp>

#include "parser.hpp"

BOOST_AUTO_TEST_CASE( version_option_test )
{
    size_t x = 0;
    size_t y = 0;
    std::string board_str;

    std::array<char const*, 2> argv =
            {{ "program_name", "-v" }};

    BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x, y, board_str),
            board_not_parsed);
}

BOOST_AUTO_TEST_CASE( help_option_test )
{
    size_t x = 0;
    size_t y = 0;
    std::string board_str;

    std::array<char const*, 2> argv =
            { "program_name", "-h" };

    BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x, y, board_str),
            board_not_parsed);
}

BOOST_AUTO_TEST_CASE( file_option_test )
{
    {
        size_t x = 0;
        size_t y = 0;
        std::string board_str;

        std::array<char const*, 2> argv =
                { "program_name", "-f" };

        BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x, y, board_str),
                std::logic_error);
    }
    {

        size_t x = 0;
        size_t y = 0;
        std::string board_str;

        std::array<char const*, 2> argv =
                { "program_name", "-f nosuchfile" };

        BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x, y, board_str),
                std::logic_error);

    }
}

BOOST_AUTO_TEST_CASE( board_option_test )
{
    {
        size_t x = 0;
        size_t y = 0;
        std::string board_str;

        std::array<char const*, 3> argv =
                { "program_name", "-x1", "-y2" };

        BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x, y, board_str),
                std::logic_error);
    }
    {
        size_t x = 0;
        size_t y = 0;
        std::string board_str;

        std::array<char const*, 4> argv =
                { "program_name", "-x1", "-y2", "-b" };

        BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x, y, board_str),
                std::logic_error);
    }
    {
        size_t x = 0;
        size_t y = 0;
        std::string board_str;

        std::array<char const*, 4> argv =
                { "program_name", "-x1", "-y2", "-bXXXX" };

        BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x, y, board_str),
                std::logic_error);
    }
}
