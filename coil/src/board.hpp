#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include <htmlcxx/html/ParserDom.h>

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

      squares& operator=(const std::string& squares)
      {
         copy_from(squares);
         return *this;
      }

   private:
      void copy_from(const std::string& squares)
      {
         this->clear();

         //std::for_each(squares.begin(), squares.end(), [](const std::string::value_type& square_str)
         for (const auto& symbol : squares)
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

   board(const size_t& width, const size_t & height, const std::string & squares)
   {
      // check board size
      if (width * height != squares.size())
      {
         throw std::logic_error("board size mismatch");
      }

      width_ = width;
      height_ = height;
      squares_ = squares;
   }

   board(boost::asio::io_service & ios, const std::string & url)
   {
      downloader_.reset(new curl::downloader(ios));
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
   boost::shared_ptr<curl::downloader> downloader_;
   size_t width_;
   size_t height_;
   squares squares_;

   void html_handler(const boost::system::error_code & ec, const std::string & html)
   {
      if (ec)
      {
         throw ec;
      }

      // parse out board from html: <param name="FlashVars" value="x=5&y=3&board=......X......X." />
      std::string html_board;

      htmlcxx::HTML::ParserDom parser;
      tree<htmlcxx::HTML::Node> dom = parser.parseTree(html);
      tree<htmlcxx::HTML::Node>::iterator it = dom.begin();
      tree<htmlcxx::HTML::Node>::iterator end = dom.end();
      for (; it != end; ++it)
      {
         if (it->tagName() == "param")
         {
            it->parseAttributes();
            if ("FlashVars" == it->attribute("name").second)
            {
               html_board = it->attribute("value").second;
            }
         }
      }

      // split to x, y and board
      std::vector<std::string> strs;
      boost::split(strs, html_board, boost::is_any_of("&"));
      std::copy(strs.begin(), strs.end(), std::ostream_iterator<std::string>(std::cout, " "));
   }
};

}
