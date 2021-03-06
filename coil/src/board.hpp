#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include <boost/optional.hpp>
#include <boost/date_time.hpp>

namespace coil
{

enum class direction
{
   up = 'U',
   down = 'D',
   left = 'L',
   right = 'R',
};

coil::direction & operator++(coil::direction & direction)
{
   //std::cout<<(char)direction << " -> ";
   switch (direction)
   {
      case coil::direction::up:
         direction = coil::direction::right;
         break;
      case coil::direction::right:
         direction = coil::direction::down;
         break;
      case coil::direction::down:
         direction = coil::direction::left;
         break;
      case coil::direction::left:
         direction = coil::direction::up;
         break;
      default:

      {
         throw std::invalid_argument("Invalid direction");
      }
   }
   //std::cout<<(char)direction << std::endl;
   return direction;
}

std::ostream & operator<<(std::ostream & os, const coil::direction & rhs)
{
   ///@TODO use underlying_type of enum
   os << static_cast<std::string::value_type>(rhs);
   return os;
}

//typedef boost::optional<direction> last_direction;

struct coord
{
   typedef size_t value_type;
   value_type x_;
   value_type y_;
   coord(const value_type & x = 0, const value_type & y = 0) :
      x_(x), y_(y)
   {

   }

   bool operator ==(const coord & rhs) const
      {
      return x_ == rhs.x_ && y_ == rhs.y_;
   }
};

std::ostream & operator<<(std::ostream & os, const coil::coord & rhs)
{
   os << "x = " << rhs.x_ << ", y = " << rhs.y_;
   return os;
}

//typedef boost::optional<coord> optional_coord;

struct board
{

//public:

   enum class cell
   {
      empty = '.',
      wall = 'X',
      step = 'O',
   };

   typedef std::vector<cell> cells;
   typedef std::vector<direction> path;

   board() :
      level_(0), width_(0), height_(0)
   {

   }

   board(const size_t & width, const size_t & height, const std::string & cells,
      const size_t & level = 0) :
      board()
   {
      if (!is_sane(width, height, cells))
      {
         throw std::invalid_argument("board size mismatch");
      }

      level_ = level;
      width_ = width;
      height_ = height;
      str_to_cells(solving_cells_, cells);
      starting_cells_ = solving_cells_;
   }

   bool operator==(const board & rhs) const

   {
      return width_ == rhs.width_ && height_ == rhs.height_ && solving_cells_ == rhs.solving_cells_;
   }

   bool operator!=(const board & rhs) const

   {
      return !(*this == rhs);
   }

   friend std::ostream & operator<<(std::ostream & os, const board & rhs);

   bool is_solved() const
   {
      // is solved if there is no empty cells
      return solving_cells_.end()
         == std::find(solving_cells_.begin(), solving_cells_.end(), cell::empty);
   }

//private:
   size_t level_;
   size_t width_;
   size_t height_;

   cells solving_cells_;
   cells starting_cells_;

   path path_;
   path qpath_;

   coord current_coord_;
   coord start_coord_;

   boost::posix_time::ptime started_solving_;
   boost::posix_time::ptime finished_solving_;

   cells & str_to_cells(cells & t, const std::string & string)
   {
      t.clear();

      for (const auto & value : string)
      {
         static_assert( boost::is_enum< cells::value_type >::value, "Not an enum" );

         cells::value_type cell = static_cast<cells::value_type>(value);
         switch (cell)
         {
            case board::cell::empty:

            case board::cell::wall:

               t.push_back(cell);
               break;
            default:

            {
               std::stringstream err;
               err << "Wrong cell value: " << value;
               throw std::invalid_argument(err.str().c_str());
            }
         }
      }
      return t;
   }

   /// try step and do step
   bool step(const coil::direction & direction)
   {
      // try step
      coord new_coord = get_new_coords(direction);
      if (!is_sane(new_coord))
      {
         // out of board
         return false;
      }

      board::cell & cell = get_cell(new_coord);
      if (board::cell::empty != cell)
      {
         // not an empty cell
         return false;
      }

      // do step
      current_coord_ = new_coord;
      cell = board::cell::step;

      return true;
   }

   /// check if could step selected direction
   bool try_step(const coil::direction & direction)
   {
      coord new_coord = get_new_coords(direction);
      if (!is_sane(new_coord))
      {
         // out of board
         return false;
      }

      board::cell & cell = get_cell(new_coord);
      if (board::cell::empty != cell)
      {
         // not an empty cell
         return false;
      }

      return true;
   }

   /// Check if could step on directions other then provided one
   bool if_other_directions_available(const coil::direction & direction)
   {
      coil::direction d = direction;

      const bool step1 = try_step(++d);
      const bool step2 = try_step(++d);
      const bool step3 = try_step(++d);

      return step1 || step2 || step3;
   }

   /// perform move (do steps until hit wall or border)
   ///@ param direction direction to move
   ///@ return @true if was moved, @false otherwise
   bool move(const coil::direction & direction)
   {
      const bool other_directions_available = if_other_directions_available(direction);
      const bool has_first_step = step(direction);

      if (has_first_step)
      {
         // try to step further the same direction
         for (; step(direction);)
         {
            // step until bumped into something
         }

         // add direction to path
         path_.push_back(direction);

         if (other_directions_available)
         {
            // and to quick path
            qpath_.push_back(direction);
         }
      }

      return has_first_step;
   }

   /// get coordinates of new step following direction
   coord get_new_coords(const coil::direction & direction)
   {
      coord new_coord = current_coord_;

      switch (direction)
      {
         case coil::direction::up:

         {
            new_coord.y_--;
            break;
         }
         case coil::direction::down:

         {
            new_coord.y_++;
            break;
         }
         case coil::direction::left:

         {
            new_coord.x_--;
            break;
         }
         case coil::direction::right:

         {
            new_coord.x_++;
            break;
         }
         default:

         {
            throw std::invalid_argument("Invalid step");
         }
      }
      return new_coord;
   }

   ///@return @true if coordinates are within board
   bool is_sane(const coord & coord) const

   {
      return coord.x_ < width_ && coord.y_ < height_;
   }

   ///@ return @true if board size matches with board width and height
   bool is_sane(const size_t& width, const size_t & height, const std::string & cells) const

   {
      return width * height == cells.size();
   }

   ///@return @true if index is within board
   bool is_sane(const cells::size_type & idx) const

   {
      return solving_cells_.size() > idx;
   }


   cells::size_type to_index(const coord & coord) const

   {
      if (!is_sane(coord))
      {
         std::stringstream serr;
         serr << "wrong coordinates: x = " << coord.x_
            << ", y = " << coord.y_;
         throw std::invalid_argument(serr.str().c_str());
      }

      return width_ * coord.y_ + coord.x_;
   }

   coord to_coord(const cells::size_type & idx) const

   {
      if (!is_sane(idx))
      {
         std::stringstream serr;
         serr << "wrong index: idx = " << idx;
         throw std::invalid_argument(serr.str().c_str());
      }

      coord::value_type x = idx % width_;
      coord::value_type y = idx / width_;

      return coord(x, y);
   }

   board::cell & get_cell(const coord & coord)
   {
      return solving_cells_.at(to_index(coord));
   }

   board::cell & get_cell(const cells::size_type & idx)
   {
      return solving_cells_.at(idx);
   }

   board::cell & get_cell(const coil::coord::value_type & x, const coil::coord::value_type & y)
   {
      return get_cell(coord(x, y));
   }

//   last_direction get_last_direction() const
//   {
//      if (path_.size())
//      {
//         return last_direction(path_.back());
//      }
//      else
//      {
//         return last_direction();
//      }
//   }
};

std::ostream & operator<<(std::ostream & os, const board::cells & cells)
{
   for (const auto & cell : cells)
   {
      os << static_cast<std::string::value_type>(cell);
   }
   //  std::copy(rhs.begin(), rhs.end(), std::ostream_iterator<board::cells::value_type>(os, " "));
   return os;
}

std::ostream & operator<<(std::ostream & os, const board::path & path)
{
   for (const auto & step : path)
   {
      os << static_cast<std::string::value_type>(step);
   }
   //  std::copy(rhs.begin(), rhs.end(), std::ostream_iterator<board::cells::value_type>(os, " "));
   return os;
}

//std::string & operator std::string(std::string & os, const board::path & path)
//   {
////   for (const auto & step : path)
////   {
////      os += static_cast<std::string::value_type>(step);
////   }
////
////   return os;
//}

std::ostream & operator<<(std::ostream & os, const board & rhs)
{
   // should be in a format that could be parsed by config file parser
   os << "level = " << rhs.level_ << std::endl;
   os << "width = " << rhs.width_ << std::endl;
   os << "height = " << rhs.height_ << std::endl;
   os << "cells = " << rhs.solving_cells_ << std::endl;
//   os << "start point = " << (rhs.start_coord_ ? rhs.start_coord_->x_ : 0)
//      << ", " << (rhs.start_coord_ ? rhs.start_coord_->y_ : 0)
//      << std::endl;
   os << "starting point = " << rhs.start_coord_.x_ << ", " << rhs.start_coord_.y_
      << std::endl;

   os << "current point = " << rhs.current_coord_.x_ << ", " << rhs.current_coord_.y_
      << std::endl;
   os << "path = " << rhs.path_ << std::endl;
   os << "short path = " << rhs.qpath_ << std::endl;
   //os << std::boolalpha << ", is solved? " << rhs.is_solved();
   os << "is solved = " << (rhs.is_solved() ? "yes" : "no") << std::endl;

   if (rhs.is_solved())
   {
      os << "solved in " << (rhs.finished_solving_ - rhs.started_solving_) << std::endl;
   }

   return os;
}

} // namespace coil
