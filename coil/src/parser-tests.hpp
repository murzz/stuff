#pragma once

#include <array>

//#define BOOST_TEST_MODULE parser tests
#include <boost/test/unit_test.hpp>

#include "parser.hpp"

namespace test
{
struct board
{
   board() :
         x_(0), y_(0)
   {
   }

   size_t x_;
   size_t y_;
   std::string squares_;
};
}

BOOST_AUTO_TEST_CASE( version_option_test )
{
   size_t x = 0;
   size_t y = 0;
   std::string squares;

   std::array<char const*, 2> argv =
         {
               { "program_name", "-v" } };

   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x, y, squares),
         board_not_parsed);
}

BOOST_AUTO_TEST_CASE( help_option_test )
{
   size_t x = 0;
   size_t y = 0;
   std::string squares;

   std::array<char const*, 2> argv =
         { "program_name", "-h" };

   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x, y, squares),
         board_not_parsed);
}

BOOST_FIXTURE_TEST_CASE( file_option_test1, test::board )
{
   std::array<char const*, 2> argv =
         { "program_name", "-f" };

   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
         std::logic_error);
}

BOOST_FIXTURE_TEST_CASE( file_option_test2, test::board )
{
   std::array<char const*, 2> argv =
         { "program_name", "-f nosuchfile" };

   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
         std::logic_error);
}

BOOST_FIXTURE_TEST_CASE( file_option_test3, test::board )
{
   std::array<char const*, 2> argv =
         { "program_name", "-f /etc/passwd" };

   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
         std::logic_error);
}

BOOST_FIXTURE_TEST_CASE( board_options_test1, test::board )
{
   std::array<char const*, 2> argv =
         { "program_name", "-x" };
   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
         std::logic_error);
}

BOOST_FIXTURE_TEST_CASE( board_options_test2, test::board )
{
   std::array<char const*, 2> argv =
         { "program_name", "-y" };
   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
         std::logic_error);
}

BOOST_FIXTURE_TEST_CASE( board_options_test3, test::board )
{
   std::array<char const*, 2> argv =
         { "program_name", "-b" };
   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
         std::logic_error);
}

BOOST_FIXTURE_TEST_CASE( board_options_test4, test::board )
{
   std::array<char const*, 3> argv =
         { "program_name", "-x", "-y" };
   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
         std::logic_error);
}

BOOST_FIXTURE_TEST_CASE( board_options_test5, test::board )
{
   std::array<char const*, 4> argv =
         { "program_name", "-x", "-y", "-b" };
   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
         std::logic_error);
}

BOOST_FIXTURE_TEST_CASE( board_options_test6, test::board )
{
   std::array<char const*, 4> argv =
         { "program_name", "-x1", "-y", "-b" };
   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
         std::logic_error);
}

BOOST_FIXTURE_TEST_CASE( board_options_test7, test::board )
{
   std::array<char const*, 4> argv =
         { "program_name", "-x1", "-y1", "-b" };
   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
         std::logic_error);
}

BOOST_FIXTURE_TEST_CASE( board_options_test8, test::board )
{
   std::array<char const*, 4> argv =
         { "program_name", "-x1", "-y1", "-b1" };
   BOOST_REQUIRE_NO_THROW(parse(argv.size(), argv.data(), x_, y_, squares_));
}

