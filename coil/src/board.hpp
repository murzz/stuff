#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include <boost/optional.hpp>

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
};

typedef boost::optional<coord> optional_coord;

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
      width_(0), height_(0)
   {

   }

   board(const size_t& width, const size_t & height, const std::string & cells) :
      board()
   {
      if (!is_sane(width, height, cells))
      {
         throw std::invalid_argument("board size mismatch");
      }

      width_ = width;
      height_ = height;
      //cells_ = cells;
      convert(cells_, cells);
   }

   bool operator==(const board & rhs) const

   {
      return width_ == rhs.width_ && height_ == rhs.height_ && cells_ == rhs.cells_;
   }

   bool operator!=(const board & rhs) const

   {
      return !(*this == rhs);
   }

   friend std::ostream & operator<<(std::ostream & os, const board & rhs);

   bool is_solved() const
   {
      // is solved if there is no empty cells
      return cells_.end() == std::find(cells_.begin(), cells_.end(), cell::empty);
   }

//private:
   size_t width_;
   size_t height_;
   cells cells_;
   path path_; ///< moves
   path qpath_; ///< moves

   void convert(cells & t, const std::string & string)
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
               throw std::invalid_argument("Wrong cell value");
         }
      }
   }

   bool step(const coil::direction & direction)
   {
      // can step
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

   bool if_other_directions_available(const coil::direction & direction)
   {
      coil::direction d = direction;

      const bool step1 = try_step(++d);
      const bool step2 = try_step(++d);
      const bool step3 = try_step(++d);

      return step1 || step2 || step3;
   }

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
            qpath_.push_back(direction);
         }
      }

      return has_first_step;
   }

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

   //typedef boost::optional<size_t> index_t;

   coord current_coord_;

   optional_coord starting_coord_;

   bool is_sane(const coord & coord) const

   {
      return coord.x_ < width_ && coord.y_ < height_;
   }

   bool is_sane(const size_t& width, const size_t & height, const std::string & cells) const

   {
      return width * height == cells.size();
   }

   size_t to_index(const coord & coord) const

   {
      if (!is_sane(coord))
      {
         throw std::invalid_argument("wrong coordinates");
      }

      return width_ * coord.y_ + coord.x_;
   }

   board::cell & get_cell(const coord & coord)
   {
      return cells_.at(to_index(coord));
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

std::ostream & operator<<(std::ostream & os, const board & rhs)
{
   // print board
   for (size_t idx = 0; idx < rhs.cells_.size(); ++idx)
   {
      os << static_cast<std::string::value_type>(rhs.cells_.at(idx));
      if ((idx + 1) % rhs.width_ == 0)
      {
         os << std::endl;
      }
   }
   os << std::endl;

   // should be in a format that could be parsed by config file parser
   os << "width = " << rhs.width_ << std::endl;
   os << "height = " << rhs.height_ << std::endl;
   os << "cells = " << rhs.cells_ << std::endl;
   os << "start point = " << (rhs.starting_coord_ ? rhs.starting_coord_->x_ : 0)
      << ", " << (rhs.starting_coord_ ? rhs.starting_coord_->y_ : 0)
      << std::endl;
   os << "current point = " << rhs.current_coord_.x_ << ", " << rhs.current_coord_.y_
      << std::endl;
   os << "path = " << rhs.path_ << std::endl;
   os << "short path = " << rhs.qpath_ << std::endl;
   //os << std::boolalpha << ", is solved? " << rhs.is_solved();
   os << "is solved = " << (rhs.is_solved() ? "yes" : "no") << std::endl;

   return os;
}

} // namespace coil
