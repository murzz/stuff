#pragma once

//#define BOOST_TEST_MODULE board tests
#include <boost/test/unit_test.hpp>

#include "board.hpp"

boost::test_tools::predicate_result
compare_boards(const coil::board & l1, const coil::board & l2)
{
   if (l1 == l2)
   {
      return true;
   }

   boost::test_tools::predicate_result res(false);
   res.message() << "boards not equal";
   return res;
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

BOOST_AUTO_TEST_CASE( very_large_board )
{
   const size_t x = 9999;
   const size_t y = 19999;
   const std::string squares(x * y, '.');
   BOOST_REQUIRE_NO_THROW(coil::board(x, y, squares));
}

BOOST_AUTO_TEST_CASE( zero_widht_board )
{
   BOOST_REQUIRE_THROW(coil::board(0, 1, "X"), std::logic_error);
}

BOOST_AUTO_TEST_CASE( wrong_cell_symbol_board )
{
   BOOST_REQUIRE_THROW(coil::board(1, 1, "A"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( dimension_and_size_mismatch )
{
   BOOST_REQUIRE_THROW(coil::board(100, 1, "X"), std::logic_error);
}
