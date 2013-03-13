#pragma once

//#define BOOST_TEST_MODULE board tests
#include <boost/test/unit_test.hpp>

#include "board.hpp"

boost::test_tools::predicate_result
compare_boards(const board& l1, const board& l2)
{
   if (l1 == l2)
   {
      return true;
   }

   boost::test_tools::predicate_result res(false);
   res.message() << "boards not equal";
   return res;
}

BOOST_AUTO_TEST_CASE( compare_boards_test1 )
{
   board b1;
   board b2;
   BOOST_REQUIRE(compare_boards(b1, b2));
}

BOOST_AUTO_TEST_CASE( compare_boards_test2 )
{
   board b1(1, 1, "X");
   board b2(1, 1, "X");
   BOOST_REQUIRE(compare_boards(b1, b2));
}

BOOST_AUTO_TEST_CASE( compare_boards_test3 )
{
   board b1(1, 1, "X");
   board b2(1, 1, ".");
   BOOST_REQUIRE(!compare_boards(b1, b2));
}

BOOST_AUTO_TEST_CASE( compare_boards_test4 )
{
   board b1(1, 2, "XX");
   board b2(2, 1, "XX");
   BOOST_REQUIRE(!compare_boards(b1, b2));
}

BOOST_AUTO_TEST_CASE( board_test1 )
{
   BOOST_REQUIRE_NO_THROW(board(1, 1, "X"));
}

BOOST_AUTO_TEST_CASE( board_test2 )
{
   BOOST_REQUIRE_NO_THROW(board(1, 2, ".X"));
}

BOOST_AUTO_TEST_CASE( board_test3 )
{
   BOOST_REQUIRE_NO_THROW(board(3, 4, "XXXXXXXXXXXX"));
}

BOOST_AUTO_TEST_CASE( board_test4 )
{
   BOOST_REQUIRE_NO_THROW(board(4, 3, "............"));
}

BOOST_AUTO_TEST_CASE( board_test5 )
{
   const size_t x = 100;
   const size_t y = 500;
   const std::string squares(x * y, 'X');
   BOOST_REQUIRE_NO_THROW(board(x, y, squares));
}

//BOOST_AUTO_TEST_CASE( board_test6 )
//{
//   const size_t x = 9999;
//   const size_t y = 19999;
//   const std::string squares(x * y, '.');
//   BOOST_REQUIRE_NO_THROW(board(x, y, squares));
//}

BOOST_AUTO_TEST_CASE( board_test10 )
{
   BOOST_REQUIRE_THROW(board(0, 1, "X"), std::logic_error);
}

BOOST_AUTO_TEST_CASE( board_test11 )
{
   BOOST_REQUIRE_THROW(board(1, 1, "A"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( board_test12 )
{
   BOOST_REQUIRE_THROW(board(100, 1, "X"), std::logic_error);
}
