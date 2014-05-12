#pragma once

#include <iostream>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

#include "board.hpp"
#include "cmdline-parser.hpp"
#include "env.hpp"

void solve(boost::asio::io_service & io_service, coil::board board);

void save(std::ostream & os, const coil::board & board)
{
   // print board for debug purposes
   const std::string line_start = "#";
   os << line_start;

   for (size_t idx = 0; idx < board.cells_.size(); ++idx)
   {
      os << static_cast<std::string::value_type>(board.cells_.at(idx));
      if ((idx + 1) % board.width_ == 0)
      {
         os << std::endl;
         if (idx + 1 != board.cells_.size())
         {
            os << line_start;
         }
      }
   }
   os << std::endl;

   os << board;
   os << "pool size = " << env::get().pool_size_ << std::endl;
}

void save(const coil::board & board)
{
   const std::string file_name = "solution_" + boost::lexical_cast<std::string>(board.level_) + "_"
      + boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());

   std::ofstream f(file_name.c_str());
   save(f, board);
}

void new_board_handler(boost::asio::io_service & io_service, const coil::board & board)
{
   io_service.post(boost::bind(solve, boost::ref(io_service), board));
}

void upload(boost::asio::io_service & io_service, const coil::board & board)
{
   ///@TODO upload board, you will get new board as a reply (if solution is ok)
   ///@TODO start with next board by starting executable again, thus no need to pass cmd line arguments

   std::stringstream qpath;
   qpath << board.qpath_;

   const std::string url = env::get().url_
      + "?name=" + env::get().name_
      + "&password=" + env::get().pass_
      + "&qpath=" + qpath.str()
      + "&x=" + boost::lexical_cast<std::string>(board.starting_coord_->x_)
      + "&y=" + boost::lexical_cast<std::string>(board.starting_coord_->y_)
         ;

   typedef boost::function<void(boost::asio::io_service & io_service, const coil::board & board)> board_handler_type;
   board_handler_type board_handler_functor = boost::bind(new_board_handler, boost::ref(io_service),
      _2);

//   typedef typename cmdline::internal::templated_functor<board_handler_type>::html_handler_type html_handler_type;
//   html_handler_type html_handler_functor = boost::bind(
//      cmdline::internal::html_handler<boost::tuple<board_handler_type>>,
//      boost::ref(io_service),
//      boost::make_tuple(board_handler_functor), _3);

//   io_service.post(
//      boost::bind(curl::download<boost::tuple<html_handler_type, board_handler_type> >,
//         boost::ref(io_service),
//         boost::make_tuple(html_handler_functor, board_handler_functor), url));

}

void move(boost::asio::io_service & io_service, coil::board board,
   const coil::direction & direction)
{
   const bool is_moved = board.move(direction);

   if (board.is_solved())
   {
      board.finished_solving_ = boost::posix_time::second_clock::local_time();
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
   // mark start point as stumped (not empty)
   board.get_cell(board.current_coord_) = coil::board::cell::step;
}

void solve(boost::asio::io_service & io_service, coil::board board)
{
   set_start_coord(board);

   // move to all 4 directions
   coil::direction direction = coil::direction::up;

   io_service.post(boost::bind(move, boost::ref(io_service), board, direction));
   io_service.post(boost::bind(move, boost::ref(io_service), board, ++direction));
   io_service.post(boost::bind(move, boost::ref(io_service), board, ++direction));
   io_service.post(boost::bind(move, boost::ref(io_service), board, ++direction));
}
