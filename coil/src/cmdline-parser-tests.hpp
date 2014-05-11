#pragma once

#include <array>
#include <fstream>
//#define BOOST_TEST_MODULE parser tests
#include <boost/test/unit_test.hpp>

#include "cmdline-parser.hpp"

//namespace test
//{
//
//struct board
//{
//   board() :
//      x_(0), y_(0)
//   {
//   }
//
//   size_t x_;
//   size_t y_;
//   std::string squares_;
//};
//
//}

BOOST_AUTO_TEST_CASE( get_board_string_from_html_test )
{
   std::ifstream file("./coil/res/level2.html");
   std::stringstream stream;
   stream << file.rdbuf();

   std::string board;
   cmdline::internal::get_board_string(board, stream.str());

   const std::string expected = "x=5&y=3&board=......X......X.&level=2";

   BOOST_REQUIRE_EQUAL(expected, board);
}

BOOST_AUTO_TEST_CASE( get_no_board_string_from_html_test )
{
   std::string board;
   const std::string html = "<html>not a mortal coil html page</html>";

   BOOST_REQUIRE_THROW(cmdline::internal::get_board_string(board, html), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE( get_wrong_board_string_from_html_test )
{
   std::ifstream file("./coil/res/level2-wrong-board.html");
   std::stringstream stream;
   stream << file.rdbuf();

   std::string board;

   BOOST_REQUIRE_THROW(cmdline::internal::get_board_string(board, stream.str()), std::invalid_argument);
}

//BOOST_AUTO_TEST_CASE( version_option_test )
//{
//   size_t x = 0;
//   size_t y = 0;
//   std::string squares;
//
//   std::array<char const*, 2> argv =
//      {
//         { "dummy_binary_name", "-v" }
//      };
//
//   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x, y, squares),
//      board_not_parsed);
//}

//BOOST_AUTO_TEST_CASE( help_option_test )
//{
//   size_t x = 0;
//   size_t y = 0;
//   std::string squares;
//
//   std::array<char const*, 2> argv =
//      { "program_name", "-h" };
//
//   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x, y, squares),
//      board_not_parsed);
//}
//
//BOOST_FIXTURE_TEST_CASE( file_option_test1, test::board )
//{
//   std::array<char const*, 2> argv =
//      { "program_name", "-f" };
//
//   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
//      std::logic_error);
//}
//
//BOOST_FIXTURE_TEST_CASE( file_option_test2, test::board )
//{
//   std::array<char const*, 2> argv =
//      { "program_name", "-fnosuchfile" };
//
//   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
//      std::logic_error);
//}
//
//BOOST_FIXTURE_TEST_CASE( file_option_test3, test::board )
//{
//   std::array<char const*, 2> argv =
//      { "program_name", "--file=/etc/passwd" };
//
//   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
//      std::logic_error);
//}
//
//BOOST_FIXTURE_TEST_CASE( file_option_test4, test::board )
//{
//   std::array<char const*, 2> argv =
//      { "program_name", "--file=/tmp/coil/2" };
//
//   BOOST_REQUIRE_NO_THROW(parse(argv.size(), argv.data(), x_, y_, squares_));
//
//   BOOST_REQUIRE_EQUAL(x_, 5);
//   BOOST_REQUIRE_EQUAL(y_, 3);
//   BOOST_REQUIRE_EQUAL(squares_, "......X......X.");
//}
//
//BOOST_FIXTURE_TEST_CASE( board_options_test1, test::board )
//{
//   std::array<char const*, 2> argv =
//      { "program_name", "--x=1" };
//   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
//      board_not_parsed);
//}
//
//BOOST_FIXTURE_TEST_CASE( board_options_test2, test::board )
//{
//   std::array<char const*, 2> argv =
//      { "program_name", "--y=1" };
//   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
//      board_not_parsed);
//}
//
//BOOST_FIXTURE_TEST_CASE( board_options_test3, test::board )
//{
//   std::array<char const*, 2> argv =
//      { "program_name", "--board=1" };
//   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
//      board_not_parsed);
//}
//
//BOOST_FIXTURE_TEST_CASE( board_options_test4, test::board )
//{
//   std::array<char const*, 3> argv =
//      { "program_name", "--x=1", "--y=1" };
//   BOOST_REQUIRE_THROW(parse(argv.size(), argv.data(), x_, y_, squares_),
//      board_not_parsed);
//}
//
//BOOST_FIXTURE_TEST_CASE( board_options_test5, test::board )
//{
//   std::array<char const*, 4> argv =
//      { "program_name", "--x=1", "--y=1", "--board=1" };
//   BOOST_REQUIRE_NO_THROW(parse(argv.size(), argv.data(), x_, y_, squares_));
//}
//
//BOOST_FIXTURE_TEST_CASE( cmdline_and_board_options_test1, test::board )
//{
//   std::array<char const*, 5> argv =
//      { "program_name", "--file=/tmp/coil/2", "--x=2", "--y=1", "--board=X" };
//
//   BOOST_REQUIRE_NO_THROW(parse(argv.size(), argv.data(), x_, y_, squares_));
//   BOOST_REQUIRE_EQUAL(x_, 2);
//   BOOST_REQUIRE_EQUAL(y_, 1);
//   BOOST_REQUIRE_EQUAL(squares_, "X");
//}
//
//BOOST_FIXTURE_TEST_CASE( cmdline_and_board_options_test2, test::board )
//{
//   std::array<char const*, 3> argv =
//      { "program_name", "--file=/tmp/coil/2", "--board=X" };
//
//   BOOST_REQUIRE_NO_THROW(parse(argv.size(), argv.data(), x_, y_, squares_));
//   BOOST_REQUIRE_EQUAL(x_, 5);
//   BOOST_REQUIRE_EQUAL(y_, 3);
//   BOOST_REQUIRE_EQUAL(squares_, "X");
//}

