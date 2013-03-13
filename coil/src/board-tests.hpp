#pragma once

//#define BOOST_TEST_MODULE board tests
#include <boost/test/unit_test.hpp>

#include "board.hpp"

//boost::test_tools::predicate_result
//compare_boards(board const& l1, board const& l2)
//{
//    if (l1.board_str_.size() != l2.board_str_.size())
//    {
//        boost::test_tools::predicate_result res(false);
//
//        res.message() << "Different sizes [" << l1.board_str_.size() << "!="
//                << l2.board_str_.size()
//                << "]";
//
//        return res;
//    }
//
//    return true;
//}

//BOOST_AUTO_TEST_CASE( f )
//{
//    board b1;
//    board b2;
//    BOOST_CHECK(compare_boards(b1, b2));
//}

