#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <curl/curl.h>
#include "board.hpp"

class downloader
{
private:
   boost::asio::io_service & io_service_;
   std::string html_;

   template<typename HTMLHandler>
   void init(const std::string & url, HTMLHandler html_handler)
   {
      CURL * curl = curl_easy_init();
      if (curl)
      {
         io_service_.post(
            boost::bind(&downloader::setopt<HTMLHandler>, this, url, curl, html_handler));
      }
      else
      {
         html_handler.template get<0>()(
            boost::asio::error::make_error_code(boost::asio::error::invalid_argument),
            std::string());
      }
   }

   template<typename HTMLHandler>
   void cleanup(const boost::system::error_code & ec, CURL * curl, HTMLHandler html_handler)
   {
      if (curl)
      {
         curl_easy_cleanup(curl);
      }
      io_service_.post(boost::bind(html_handler.template get<0>(), ec, std::string()));
      //html_handler.template get<0>()(ec, std::string());
   }

   template<typename HTMLHandler>
   void setopt(const std::string & url, CURL * curl, HTMLHandler html_handler)
   {
      CURLcode res = CURLE_OK;
      res = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      if (CURLE_OK != res)
      {
         io_service_.post(
            boost::bind(&downloader::cleanup<HTMLHandler>, this, make_error_code(res), curl,
               html_handler));
         return;
      }

      res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &downloader::write_data);
      if (CURLE_OK != res)
      {
         io_service_.post(
            boost::bind(&downloader::cleanup<HTMLHandler>, this, make_error_code(res), curl,
               html_handler));
         return;
      }

      res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
      if (CURLE_OK != res)
      {
         io_service_.post(
            boost::bind(&downloader::cleanup<HTMLHandler>, this, make_error_code(res), curl,
               html_handler));
         return;
      }

      io_service_.post(
         boost::bind(&downloader::perform<HTMLHandler>, this, curl, html_handler));
   }

   static size_t write_data(void *ptr, size_t size, size_t nmemb, downloader * self)
   {
      std::string html(static_cast<std::string::value_type*>(ptr), size * nmemb);
      self->on_html(html);
      return html.size();
   }

   void on_html(const std::string & html)
   {
      html_ += html;
   }

   template<typename HTMLHandler>
   void perform(CURL * curl, HTMLHandler html_handler)
   {
      // would block and do the job
      // this would eliminate all asio fun though
      const CURLcode res = curl_easy_perform(curl);
      if (CURLE_OK == res)
      {
         // success
         const boost::system::error_code ec;
         io_service_.post(boost::bind(html_handler.template get<0>(), ec, html_));
         //html_handler.template get<0>()(ec, html);
      }
      else
      {
         io_service_.post(
            boost::bind(&downloader::cleanup<HTMLHandler>, this,
               make_error_code(res), curl, html_handler));
      }
   }

public:
   downloader(boost::asio::io_service & io_service) :
      io_service_(io_service)
   {

   }

   template<typename HTMLHandler>
   void get_html(const std::string & url, HTMLHandler html_handler)
   {
      html_.clear();
      io_service_.post(
         boost::bind(&downloader::init<boost::tuple<HTMLHandler> >, this, url,
            boost::make_tuple(html_handler)));
   }

   class category: public boost::system::error_category
   {
   public:
      const char* name() const
      {
         return "downloader.curl";
      }

      std::string message(int value) const

      {
         return curl_easy_strerror(static_cast<CURLcode>(value));
      }
   };

   inline static const boost::system::error_category& get_error_category()
   {
      static category instance;
      return instance;
   }

//   template<class T>
//   struct is_error_code_enum
//   {
//      static const bool value = false;
//   };
//
//   template<> struct is_error_code_enum<CURLcode>
//   {
//      static const bool value = true;
//   };

   inline static boost::system::error_code make_error_code(CURLcode e)
   {
      return boost::system::error_code(
         static_cast<int>(e), get_error_category());
   }
};

// eto svjo kollbeki, kotorie zovutsja esli ne bilo errora
// nastojaschij runner budet v teste, tam instance io_Servisa
//std::string download_html()
//{
//
//}
//
//board parse_html(const std::string & html)
//{
//   return board;
//}
//
//board get_board()
//{
//   return parse_html(download_html());
//}
