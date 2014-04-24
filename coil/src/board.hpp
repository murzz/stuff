#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace coil
{
class board
{

public:

   enum class square
   {
      empty = '.',
      wall = 'X'
   };

   class squares: public std::vector<board::square>
   {

   public:
      squares()
      {
      }

      squares(const std::string& squares)
      {
         *this = squares;
      }

      squares& operator=(const std::string& string)
      {
         this->clear();

         //std::for_each(squares.begin(), squares.end(), [](const std::string::value_type& square_str)
         for (const auto& symbol : string)
         {
            board::square square = static_cast<board::square>(symbol);
            switch (square)
            {
               case board::square::empty:
                  case board::square::wall:
                  this->push_back(square);
                  break;
               default:
                  throw std::runtime_error("wrong square");
            }
         }
         // );

         return *this;
      }
   };

   board() :
      width_(0), height_(0)
   {

   }

   board(const size_t& width, const size_t & height, const std::string & string)
   {
      // check board size
      if (width * height != string.size())
      {
         throw std::logic_error("board size mismatch");
      }

      width_ = width;
      height_ = height;
      squares_ = string;
   }

   bool operator==(const board & rhs) const

   {
      return width_ == rhs.width_ && height_ == rhs.height_ && squares_ == rhs.squares_;
   }

   bool operator!=(const board & rhs) const
      {
      return !(*this == rhs);
   }

private:
   size_t width_;
   size_t height_;
   squares squares_;
};

}
