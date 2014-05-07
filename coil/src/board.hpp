#pragma once

#include <string>
#include <vector>
#include <stdexcept>

namespace coil
{

class board
{

public:

   enum class cell
   {
      empty = '.',
      wall = 'X',
      step = 'O',
   };

   enum class step
   {
      up = 'U',
      down = 'D',
      left = 'L',
      right = 'R',
   };

   typedef std::vector<cell> cells;
   typedef std::vector<step> path;

   board() :
      width_(0), height_(0), x_(0), y_(0)
   {

   }

   board(const size_t& width, const size_t & height, const std::string & cells) :
      board()
   {
      // heck board size (sanity check)
      if (width * height != cells.size())
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

   std::string solution()
   {
      return "";
   }

   std::size_t & x()
   {
      return x_;
   }

   bool move()
   {
      init();

      if (!move(step::up))
      {
         if (!move(step::right))
         {
            if (!move(step::down))
            {
               if (!move(step::left))
               {
                  // nowhere to move
                  return false;
               }
            }
         }
      }
      return true;
   }

   bool is_solved() const
   {
      // is solved if there is no empty cells
      return cells_.end() == std::find(cells_.begin(), cells_.end(), cell::empty);
   }

private:
   size_t width_;
   size_t height_;
   cells cells_;
   path path_; ///< moves
   std::size_t x_; ///< starting x coordinate
   std::size_t y_; ///< starting x coordinate

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

   ///@ param step direction to move
   ///@ return @true if was moved, @false otherwise
   bool move(const board::step & step)
   {
      // update new x and y, detect walls, update path
      return false;
   }

   void init()
   {
      // get velocity vector
      if (!path_.size())
      {
         // no velocity vector, figure it out
         path_.push_back(step::up);
      }
   }
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
   os << "width = " << rhs.width_;
   os << ", height = " << rhs.height_;
   os << ", cells = " << rhs.cells_;
   os << ", path = " << rhs.path_;
   //os << std::boolalpha << ", is solved? " << rhs.is_solved();
   os << ", is solved? " << (rhs.is_solved() ? "yes!" : "no :(");

   return os;
}

} // namespace coil
