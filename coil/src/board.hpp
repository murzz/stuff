#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

class board
{

public:

   class squares
   {

   public:

      enum class square
      {
         empty = '.',
         wall = 'X'
      };
      typedef std::vector<square> type;

      squares()
      {

      }

      squares(const std::string& squares)
      {
         *this = squares;
      }

      squares& operator=(const std::string& squares)
      {
         value_.clear();

         //std::for_each(squares.begin(), squares.end(), [](const std::string::value_type& square_str)
         for (const auto& square_str : squares)
         {
            squares::square square = static_cast<squares::square>(square_str);
            switch (square)
            {
               case squares::square::empty:
                  case squares::square::wall:
                  value_.push_back(square);
               break;
               default:
                  throw std::runtime_error("wrong square");
            }
         }
         // );

         return *this;
      }

      bool operator==(const squares& rhs) const
      {
         return value_ == rhs.value_;
      }

      type& value()
      {
         return value_;
      }

   private:
      type value_;
   };

   board() :
         x_(0), y_(0)
   {

   }

   board(const size_t& x, const size_t& y, const std::string& squares)
   {
      // check board size
      if (x * y != squares.size())
      {
         throw std::logic_error("board size mismatch");
      }

      x_ = x;
      y_ = y;
      squares_ = squares;
   }

   bool operator==(const board& rhs) const
   {
      return x_ == rhs.x_ && y_ == rhs.y_ && squares_ == rhs.squares_;
   }

   bool operator!=(const board& rhs) const
   {
      return !(*this == rhs);
   }

private:
   size_t x_;
   size_t y_;
   squares squares_;
};
