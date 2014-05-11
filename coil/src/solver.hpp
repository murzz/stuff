#pragma once

#include <iostream>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "board.hpp"

void solve(boost::asio::io_service & io_service, coil::board board);

void save(const coil::board & board)
{
   const std::string file_name = boost::posix_time::to_simple_string(
      boost::posix_time::second_clock::local_time());

   std::ofstream f(file_name.c_str());

   f << board << std::endl;
}

void upload(boost::asio::io_service & io_service, const coil::board & board)
{
   ///@TODO upload board, you will get new board as a reply (if solution is ok)
   ///@TODO start with next board by starting executable again, thus no need to pass cmd line arguments

   ///@TODO make url
   io_service.post(boost::bind(curl::download()));
}

void move(boost::asio::io_service & io_service, coil::board board,
   const coil::direction & direction)
{
   const bool is_moved = board.move(direction);

   if (board.is_solved())
   {
      io_service.post(boost::bind(save, board));
      io_service.post(boost::bind(upload, boost::ref(io_service), board));
      return;
   }

   if (is_moved)
   {
      // schedule next iteration
      io_service.post(boost::bind(solve, boost::ref(io_service), board));
      return;
   }

   // can't move, abandon board
   //std::cout << "nowhere to move" << std::endl;
   //std::cout << board << std::endl;

   //std::cout << "failed to solve" << std::endl;
//   std::cout << board << std::endl;
//   io_service.post(boost::bind(&boost::asio::io_service::stop, &io_service));
}

void set_start_coord(coil::board & board)
{
   if (board.starting_coord_)
   {
      // already set
      return;
   }

   // init starting coords
   // define starting coords, could be random
   board.starting_coord_ = coil::coord(0, 0);

   // find sane point closest to starting coord
   // this algo is looking by incrementing indexes
   // so starting point should not be too close to the edge of the board, or it would fail
   for (coil::coord::value_type x = board.starting_coord_->x_; x < board.width_; ++x)
   {
      board.starting_coord_->x_ = x;
      for (coil::coord::value_type y = board.starting_coord_->y_; y < board.height_; ++y)
      {
         board.starting_coord_->y_ = y;
         if (coil::board::cell::empty == board.get_cell(*board.starting_coord_))
         {
            break;
         }
      }
   }

   // set current coord so board::move would use it
   board.current_coord_ = *board.starting_coord_;
   // mark start point as stomped (not empty)
   board.get_cell(board.current_coord_) = coil::board::cell::step;
}

void solve(boost::asio::io_service & io_service, coil::board board)
{
   set_start_coord(board);

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
