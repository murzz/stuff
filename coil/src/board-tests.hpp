#pragma once

//#define BOOST_TEST_MODULE board
#include <boost/test/unit_test.hpp>
#include <boost/scoped_ptr.hpp>

#include "board.hpp"

boost::test_tools::predicate_result
compare_boards(const coil::board & l1, const coil::board & l2)
{
   if (l1 == l2)
   {
      return true;
   }

   boost::test_tools::predicate_result is_equal(false);
   is_equal.message() << "boards are not equal";
   return is_equal;
}

BOOST_AUTO_TEST_CASE( compare_empty_boards )
{
   BOOST_REQUIRE(compare_boards(coil::board(), coil::board()));
}

BOOST_AUTO_TEST_CASE( compare_equal_boards )
{
   coil::board b1(1, 1, "X");
   coil::board b2(1, 1, "X");
   BOOST_REQUIRE(compare_boards(b1, b2));
}

BOOST_AUTO_TEST_CASE( compare_non_equal_by_content_boards )
{
   coil::board b1(1, 1, "X");
   coil::board b2(1, 1, ".");
   BOOST_REQUIRE(!compare_boards(b1, b2));
}

BOOST_AUTO_TEST_CASE( compare_non_equal_by_size_boards )
{
   coil::board b1(1, 2, "XX"); // vertical board
   coil::board b2(2, 1, "XX"); // horizontal board
   BOOST_REQUIRE(!compare_boards(b1, b2));
}

BOOST_AUTO_TEST_CASE( create_one_square_board )
{
   BOOST_REQUIRE_NO_THROW(coil::board(1, 1, "X"));
}

BOOST_AUTO_TEST_CASE( create_two_square_board )
{
   BOOST_REQUIRE_NO_THROW(coil::board(1, 2, ".X"));
}

BOOST_AUTO_TEST_CASE( creat_x_board )
{
   BOOST_REQUIRE_NO_THROW(coil::board(3, 4, "XXXXXXXXXXXX"));
}

BOOST_AUTO_TEST_CASE( create_dot_board )
{
   BOOST_REQUIRE_NO_THROW(coil::board(4, 3, "............"));
}

BOOST_AUTO_TEST_CASE( create_large_x_board )
{
   const size_t x = 100;
   const size_t y = 500;
   const std::string squares(x * y, 'X');
   BOOST_REQUIRE_NO_THROW(coil::board(x, y, squares));
}

#ifndef DISABLE_LONG_TESTS

BOOST_AUTO_TEST_CASE( very_large_board )
{
   const size_t x = 9999;
   const size_t y = 19999;
   const std::string squares(x * y, '.');
   BOOST_REQUIRE_NO_THROW(coil::board(x, y, squares));
}

#endif // #ifndef DISABLE_LONG_TESTS

BOOST_AUTO_TEST_CASE( zero_widht_board )
{
   BOOST_REQUIRE_THROW(coil::board(0, 1, "X"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE( wrong_cell_symbol_board )
{
   BOOST_REQUIRE_THROW(coil::board(1, 1, "A"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE( dimension_and_size_mismatch )
{
   BOOST_REQUIRE_THROW(coil::board(100, 1, "X"), std::invalid_argument);
}

//BOOST_AUTO_TEST_CASE( create_and_ostream_board )
//{
//   const std::size_t width = 5, height = 3;
//   const std::string cells = "......X......X.";
//   coil::board b(width, height, cells);
//
//   std::stringstream expected;
//   expected << "width = " << width
//      << ", height = " << height
//      << ", cells = " << cells
//      << ", path = , is solved? no :(";
//
//   std::stringstream received;
//   received << b;
//
//   BOOST_REQUIRE_EQUAL(received.str(), expected.str());
//}

BOOST_AUTO_TEST_CASE( is_board_solved )
{
   const std::size_t width = 5, height = 3;
   const std::string cells = "......X......X.";
   coil::board b(width, height, cells);

   BOOST_REQUIRE_EQUAL(b.is_solved(), false);
}
