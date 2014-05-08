#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include "board.hpp"

void solve(boost::asio::io_service & io_service, coil::board board);

void move(boost::asio::io_service & io_service, coil::board board,
   const coil::direction & direction)
{
   const bool is_moved = board.move(direction);

   if (board.is_solved())
   {
      ///@TODO upload board, save it for reference, download next board
      std::cout << board << std::endl;
      return;
   }

   if (is_moved)
   {
      // schedule next move
      io_service.post(boost::bind(solve, boost::ref(io_service), board));
      return;
   }

   // can't move, abandon board

   //std::cout << "failed to solve" << std::endl;
//   std::cout << board << std::endl;
//   io_service.post(boost::bind(&boost::asio::io_service::stop, &io_service));
}

void solve(boost::asio::io_service & io_service, coil::board board)
{
   if (!board.starting_coord_)
   {
      // define starting coords, could be random
      board.starting_coord_->x = 0;
      board.starting_coord_->y = 0;
   }

   // move to all 4 directions
   coil::direction direction = coil::direction::up;

   io_service.post(boost::bind(move, boost::ref(io_service), board, direction));

   ++direction;
   io_service.post(boost::bind(move, boost::ref(io_service), board, direction));

   ++direction;
   io_service.post(boost::bind(move, boost::ref(io_service), board, direction));

   ++direction;
   io_service.post(boost::bind(move, boost::ref(io_service), board, direction));
}
