#pragma once

#include <iostream>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>

#include "board.hpp"
#include "cmdline-parser.hpp"
#include "env.hpp"

void solve(boost::asio::io_service & io_service, coil::board board);
void next_move(boost::asio::io_service & io_service, coil::board & board,
   coil::direction direction);

void save(std::ostream & os, const coil::board & board)
{
   // print board for debug purposes
   const std::string line_start = "#";
   os << line_start;

   for (size_t idx = 0; idx < board.solving_cells_.size(); ++idx)
   {
      os << static_cast<std::string::value_type>(board.solving_cells_.at(idx));
      if ((idx + 1) % board.width_ == 0)
      {
         os << std::endl;
         if (idx + 1 != board.solving_cells_.size())
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
   BOOST_LOG_TRIVIAL(info)<< "Saving solution for level #" << board.level_;

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
   // upload board, you will get new board as a reply (if solution is ok)

   BOOST_LOG_TRIVIAL(info)<< "Uploading solution for level #" << board.level_;

   std::stringstream qpath;
   qpath << board.qpath_;

   const std::string url = env::get().url_
   + "?name=" + env::get().name_
   + "&password=" + env::get().pass_
   + "&qpath=" + qpath.str()
   + "&x=" + boost::lexical_cast<std::string>(board.start_coord_.x_)
   + "&y=" + boost::lexical_cast<std::string>(board.start_coord_.y_);

   typedef boost::function<void(const coil::board & board)> board_handler_type;
   board_handler_type board_handler_functor = boost::bind(new_board_handler, boost::ref(io_service),
      _1);

   typedef boost::function<
   void(boost::asio::io_service & io_service, board_handler_type board_handler,
      const std::string & html)> html_handler_type;

   html_handler_type html_handler_functor = boost::bind(
      cmdline::internal::html_handler<boost::tuple<board_handler_type>>,
      boost::ref(io_service),
      boost::make_tuple(board_handler_functor), _3);

   io_service.post(
      boost::bind(curl::download<boost::tuple<html_handler_type, board_handler_type> >,
         boost::ref(io_service),
         boost::make_tuple(html_handler_functor, board_handler_functor), url));
}

void move(boost::asio::io_service & io_service, coil::board board,
   const coil::direction & direction)
{
   //BOOST_LOG_TRIVIAL(trace)<< "Moving " << direction;

   const bool is_moved = board.move(direction);

   if (board.is_solved())
   {
      BOOST_LOG_TRIVIAL(info)<< "Solved";
      board.finished_solving_ = boost::posix_time::second_clock::local_time();
      io_service.post(boost::bind(save, board));
      io_service.post(boost::bind(upload, boost::ref(io_service), board));
      return;
   }

   if (is_moved)
   {
      // schedule next iteration
      io_service.post(boost::bind(next_move, boost::ref(io_service), board, direction));
      return;
   }

   // can't move and unsolved
   //std::cout << "can't move " << direction << std::endl;
   //save(std::cout, board);
   //io_service.post(boost::bind(solve, boost::ref(io_service), board));
}

void next_move(boost::asio::io_service & io_service, coil::board & board,
   coil::direction direction)
{
   // move to all 4 directions at once
   io_service.post(boost::bind(move, boost::ref(io_service), board, direction));
   io_service.post(boost::bind(move, boost::ref(io_service), board, ++direction));
   io_service.post(boost::bind(move, boost::ref(io_service), board, ++direction));
   io_service.post(boost::bind(move, boost::ref(io_service), board, ++direction));
}

void solve(boost::asio::io_service & io_service, coil::board board)
{
   // set starting coord
   bool was_set = false;
   for (coil::board::cells::size_type idx = 0; idx < board.starting_cells_.size(); ++idx)
   {
      coil::board::cell & starting_cell = board.starting_cells_.at(idx);
      if (coil::board::cell::empty == starting_cell)
      {
         board.start_coord_ = board.to_coord(idx);
         board.current_coord_ = board.start_coord_;
         starting_cell = coil::board::cell::step; // mark as occupied on starting board
         board.solving_cells_.at(idx) = coil::board::cell::step; // mark as occupied on solving board
         was_set = true;
         BOOST_LOG_TRIVIAL(info)<< "Starting coords: " << board.start_coord_;

         ///@TODO could be random
         coil::direction direction = coil::direction::up;

         // make first move
         io_service.post(boost::bind(next_move, boost::ref(io_service), board, direction));
         break;
      }
   }

   if (!was_set)
   {
      BOOST_LOG_TRIVIAL(error)<< "No more starting coordinates available, unable to solve board";
      return;
   }

//   ///@TODO could be random
//   coil::direction direction = coil::direction::up;
//
//   // make first move
//   io_service.post(boost::bind(next_move, boost::ref(io_service), board, direction));
}
