#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <curl/curl.h>

namespace curl
{

namespace error
{

class category: public boost::system::error_category
{
public:
   const char* name() const noexcept
   {
      return "downloader.curl";
   }

   std::string message(int value) const

   {
      return curl_easy_strerror(static_cast<CURLcode>(value));
   }
};

} // namespace error

template<class T>
struct is_error_code_enum
{
   static const bool value = false;
};

template<>
struct is_error_code_enum<CURLcode>
{
   static const bool value = true;
};

inline static const boost::system::error_category& get_error_category()
{
   static error::category instance;
   return instance;
}

namespace error
{

inline static boost::system::error_code make_error_code(CURLcode e)
{
   return boost::system::error_code(
      static_cast<int>(e), curl::get_error_category());
}

} // namespace error

class downloader
{
private:
   boost::asio::io_service & io_service_;
   std::string content_;

   template<typename Handler>
   void init(const std::string & url, Handler handler)
   {
      CURL * curl = curl_easy_init();
      if (curl)
      {
         io_service_.post(
            boost::bind(&downloader::setopt<Handler>, this, url, curl, handler));
      }
      else
      {
         handler.template get<0>()(
            boost::asio::error::make_error_code(boost::asio::error::invalid_argument),
            std::string());
      }
   }

   template<typename Handler>
   void cleanup(const boost::system::error_code & ec, CURL * curl, Handler handler)
   {
      if (curl)
      {
         curl_easy_cleanup(curl);
      }
      io_service_.post(boost::bind(handler.template get<0>(), ec, std::string()));
   }

   template<typename Handler>
   void setopt(const std::string & url, CURL * curl, Handler handler)
   {
      CURLcode res = CURLE_OK;
      res = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      if (CURLE_OK != res)
      {
         io_service_.post(
            boost::bind(&downloader::cleanup<Handler>, this, error::make_error_code(res), curl,
               handler));
         return;
      }

      res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &downloader::write_data);
      if (CURLE_OK != res)
      {
         io_service_.post(
            boost::bind(&downloader::cleanup<Handler>, this, error::make_error_code(res), curl,
               handler));
         return;
      }

      res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
      if (CURLE_OK != res)
      {
         io_service_.post(
            boost::bind(&downloader::cleanup<Handler>, this, error::make_error_code(res), curl,
               handler));
         return;
      }

      io_service_.post(
         boost::bind(&downloader::perform<Handler>, this, curl, handler));
   }

   static size_t write_data(void *ptr, size_t size, size_t nmemb, downloader * self)
   {
      std::string content(static_cast<std::string::value_type*>(ptr), size * nmemb);
      self->on_content(content);
      return content.size();
   }

   void on_content(const std::string & content)
   {
      content_ += content;
   }

   template<typename Handler>
   void perform(CURL * curl, Handler handler)
   {
      // would block and do the job
      // this would eliminate all asio fun though
      const CURLcode res = curl_easy_perform(curl);
      if (CURLE_OK == res)
      {
         // success
         const boost::system::error_code ec;
         io_service_.post(boost::bind(handler.template get<0>(), ec, content_));
      }
      else
      {
         io_service_.post(
            boost::bind(&downloader::cleanup<Handler>, this,
               error::make_error_code(res), curl, handler));
      }
   }

public:
   downloader(boost::asio::io_service & io_service) :
      io_service_(io_service)
   {

   }

   template<typename Handler>
   void get_content(const std::string & url, Handler handler)
   {
      content_.clear();
      io_service_.post(
         boost::bind(&downloader::init<boost::tuple<Handler> >, this, url,
            boost::make_tuple(handler)));
   }

};

} //namespace curl
