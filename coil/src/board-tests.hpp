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

BOOST_AUTO_TEST_CASE( compare_boards_test )
{
   board b1;
   board b2;
   BOOST_CHECK(compare_boards(b1, b2));
}

