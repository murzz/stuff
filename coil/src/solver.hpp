#pragma once

//#include <iostream>
#include <boost/asio.hpp>
#include "board.hpp"

void solve(boost::asio::io_service & io_service, coil::board board)
{
//    std::cout << "solved" << std::endl;

// OR solve it here, instead of in board::move


   if (board.move())
   {
      // schedule next move
      io_service.post(boost::bind(solve, boost::ref(io_service), board));
      return;
   }

   // can't move
   if (board.is_solved())
   {
      ///@TODO upload board, save it for reference, download next board
      std::cout << board << std::endl;
      return;
   }

   ///@TODO failed to solve, abandon thread!
   //std::cout << "failed to solve" << std::endl;
   std::cout << board << std::endl;
   io_service.post(boost::bind(&boost::asio::io_service::stop, &io_service));
}
