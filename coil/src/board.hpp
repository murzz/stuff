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
      width_(0), height_(0)
   {

   }

   board(const size_t& width, const size_t & height, const std::string & cells)
   {
      // check board size
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

private:
   size_t width_;
   size_t height_;
   cells cells_;
   path path_;

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
};

} // namespace coil
