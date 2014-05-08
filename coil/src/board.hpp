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
   return direction;
}

//typedef boost::optional<direction> last_direction;

struct coord
{
   size_t x;
   size_t y;
   coord() :
      x(0), y(0)
   {

   }
};
typedef boost::optional<coord> starting_coord;

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

   ///@ param direction direction to move
   ///@ return @true if was moved, @false otherwise
   bool move(const coil::direction & direction)
   {
      coord new_coords = get_new_coords(direction);
      if (!is_sane(new_coords))
      {
         return false;
      }
      //  init();
      ///@TODO update new x and y, detect walls, update path
//
//      const index_t current_idx = coordinates_to_index(x_, y_);
//      if (!current_idx)
//      {
//         return false;
//      }
//
//      size_t new_x = x_;
//      size_t new_y = y_;
//      //const index_t new_idx = move(step, x_, y_);
//      if (!new_idx)
//      {
//         return false;
//      }

      bool moved = true;
      return moved;
   }

   coord get_new_coords(const direction & step)
   {

      ///@TODO we should start either from starting_coord or from current_coord
      coord new_coord = current_coord_;

      ///@TODO it should slide all cells until occupied space, instead of moving by cell like it is implemented now
      // it could be done by recursive call to this method

      //
      switch (step)
      {
         case direction::up:

         {
            new_coord.y--;
            break;
         }
         case direction::down:

         {
            new_coord.y++;
            break;
         }
         case direction::left:

         {
            new_coord.x--;
            break;
         }
         case direction::right:

         {
            new_coord.x++;
            break;
         }
         default:

         {
            throw std::invalid_argument("Invalid step");
         }
      }
      return new_coord;
   }

   typedef boost::optional<size_t> index_t;

   coord current_coord_;

   starting_coord starting_coord_;

   bool is_sane(const coord & coord) const

   {
      return coord.x < width_ && coord.y < height_;
   }

   bool is_sane(const size_t& width, const size_t & height, const std::string & cells) const

   {
      return width * height == cells.size();
   }

   index_t to_index(const coord & coord) const

   {
      if (!is_sane(coord))
      {
         return index_t();
      }

      return width_ * coord.y + coord.x;
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
   // should be in a format that could be parsed by config file parser
   os << "width = " << rhs.width_ << std::endl;
   os << "height = " << rhs.height_ << std::endl;
   os << "cells = " << rhs.cells_ << std::endl;
   os << "path = " << rhs.path_ << std::endl;
   //os << std::boolalpha << ", is solved? " << rhs.is_solved();
   os << "is solved = " << (rhs.is_solved() ? "yes!" : "no :(") << std::endl;

   return os;
}

} // namespace coil
