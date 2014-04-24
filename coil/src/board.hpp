#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

#include "downloader.hpp"

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
         copy_from(string);
         return *this;
      }

   private:
      void copy_from(const std::string& string)
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

   board(boost::asio::io_service & ios, const std::string & url)
   {
      downloader_.reset(new downloader(ios));
      downloader_->get_content(url, boost::bind(&board::html_handler, this, _1, _2));
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
   boost::shared_ptr<downloader> downloader_;
   size_t width_;
   size_t height_;
   squares squares_;

   void html_handler(const boost::system::error_code & ec, const std::string & html)
   {
      if (ec)
      {
         throw ec;
      }
     // std::cout << html;
      // no errors
//      boost::system::error_code no_error;
//      BOOST_REQUIRE_MESSAGE(ec == no_error, ec.message());
//
//      // html is here
//      const bool is_html = std::string::npos != html.find("<html>")
//         || std::string::npos != html.find("<HTML>");
//      BOOST_REQUIRE_EQUAL(is_html, true);
   }
};

}
