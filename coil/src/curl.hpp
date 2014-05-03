#pragma once

#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
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

namespace internal
{

struct write_data
{
   std::string html_;
   write_data()
   {
      std::cout << __func__ << std::endl;
   }
   write_data(const write_data& that)
   {
      html_ = that.html_;
      std::cout << __func__ << std::endl;
   }
   ~write_data()
   {
      std::cout << __func__ << std::endl;
   }
   void operator=(const write_data& that)
   {
      std::cout << __func__ << std::endl;
      html_ = that.html_;
   }
};

static size_t write_data_handler(void *ptr, size_t size, size_t nmemb, write_data * p)
{
   std::string content(static_cast<std::string::value_type*>(ptr), size * nmemb);
   //**p += content;
   //(*p)->operator +=(content);
   //(*p)->html_ += content;
   p->html_ += content;
   return content.size();
}

void cleanup(const boost::system::error_code & ec, CURL * curl)
{
   if (curl)
   {
      curl_easy_cleanup(curl);
   }

   if (ec)
   {
      throw ec;
   }
}

template<typename Handler>
void perform(boost::asio::io_service & io_service, Handler handler, CURL * curl,
   write_data * html)
{
   // would block and do the job
   // this would eliminate all asio fun though
   const CURLcode res = curl_easy_perform(curl);
   if (CURLE_OK == res)
   {
      // success
      const boost::system::error_code ec;
      io_service.post(boost::bind(cleanup, ec, curl));

//      boost::shared_ptr<std::stringstream> ss(boost::make_shared<std::stringstream>());
//      boost::shared_ptr<std::stringstream> ss(boost::make_shared<std::stringstream>(html->c_str()));
//      io_service.post(
//         boost::bind(handler.template get<0>(), boost::ref(io_service), handler.template get<1>(),
//            0, nullptr, ss));
      io_service.post(
         boost::bind(handler.template get<0>(), boost::ref(io_service), handler.template get<1>(),
            html->html_));
   }
   else
   {
      io_service.post(boost::bind(cleanup, error::make_error_code(res), curl));
   }
}

template<typename Handler>
void setopt(boost::asio::io_service & io_service, Handler handler, const std::string & url,
   CURL * curl)
{
   CURLcode res = CURLE_OK;
   res = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
   if (CURLE_OK != res)
   {
      io_service.post(boost::bind(cleanup, error::make_error_code(res), curl));
      return;
   }

   res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data_handler);
   if (CURLE_OK != res)
   {
      io_service.post(boost::bind(cleanup, error::make_error_code(res), curl));
      return;
   }

   //TODO make use of shared_ptr
   //boost::shared_ptr<std::string> html(boost::make_shared<std::string>());
   //boost::shared_ptr<write_data_handler> html(boost::make_shared<write_data_handler>());
   write_data * html = new write_data;

   res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, html);
   if (CURLE_OK != res)
   {
      io_service.post(boost::bind(cleanup, error::make_error_code(res), curl));
      return;
   }

   io_service.post(
      boost::bind(perform<Handler>, boost::ref(io_service), handler, curl, html));
}

template<typename Handler>
void init(boost::asio::io_service & io_service, Handler handler, const std::string & url)
{
   CURL * curl = curl_easy_init();
   if (curl)
   {
      io_service.post(boost::bind(setopt<Handler>, boost::ref(io_service), handler, url, curl));
   }
   else
   {
      throw boost::asio::error::make_error_code(boost::asio::error::invalid_argument);
   }
}

} //namespace internal

template<typename Handler>
void download(boost::asio::io_service & io_service, Handler handler, const std::string & url)
{
   io_service.post(
      boost::bind(internal::init<Handler>, boost::ref(io_service), handler, url));
}

//class downloader
//{
//private:
//   boost::asio::io_service & io_service_;
//   std::string content_;
//
//   template<typename Handler>
//   void init(const std::string & url, Handler handler)
//   {
//      CURL * curl = curl_easy_init();
//      if (curl)
//      {
//         io_service_.post(
//            boost::bind(&downloader::setopt<Handler>, this, url, curl, handler));
//      }
//      else
//      {
//         handler.template get<0>()(
//            boost::asio::error::make_error_code(boost::asio::error::invalid_argument),
//            std::string());
//      }
//   }
//
//   template<typename Handler>
//   void cleanup(const boost::system::error_code & ec, CURL * curl, Handler handler)
//   {
//      if (curl)
//      {
//         curl_easy_cleanup(curl);
//      }
//      io_service_.post(boost::bind(handler.template get<0>(), ec, std::string()));
//   }
//
//   template<typename Handler>
//   void setopt(const std::string & url, CURL * curl, Handler handler)
//   {
//      CURLcode res = CURLE_OK;
//      res = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//      if (CURLE_OK != res)
//      {
//         io_service_.post(
//            boost::bind(&downloader::cleanup<Handler>, this, error::make_error_code(res), curl,
//               handler));
//         return;
//      }
//
//      res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &downloader::write_data);
//      if (CURLE_OK != res)
//      {
//         io_service_.post(
//            boost::bind(&downloader::cleanup<Handler>, this, error::make_error_code(res), curl,
//               handler));
//         return;
//      }
//
//      res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
//      if (CURLE_OK != res)
//      {
//         io_service_.post(
//            boost::bind(&downloader::cleanup<Handler>, this, error::make_error_code(res), curl,
//               handler));
//         return;
//      }
//
//      io_service_.post(
//         boost::bind(&downloader::perform<Handler>, this, curl, handler));
//   }
//
//   static size_t write_data(void *ptr, size_t size, size_t nmemb, downloader * self)
//   {
//      std::string content(static_cast<std::string::value_type*>(ptr), size * nmemb);
//      self->on_content(content);
//      return content.size();
//   }
//
//   void on_content(const std::string & content)
//   {
//      content_ += content;
//   }
//
//   template<typename Handler>
//   void perform(CURL * curl, Handler handler)
//   {
//      // would block and do the job
//      // this would eliminate all asio fun though
//      const CURLcode res = curl_easy_perform(curl);
//      if (CURLE_OK == res)
//      {
//         // success
//         const boost::system::error_code ec;
//         io_service_.post(boost::bind(handler.template get<0>(), ec, content_));
//      }
//      else
//      {
//         io_service_.post(
//            boost::bind(&downloader::cleanup<Handler>, this,
//               error::make_error_code(res), curl, handler));
//      }
//   }
//
//public:
//   downloader(boost::asio::io_service & io_service) :
//      io_service_(io_service)
//   {
//
//   }
//
//   template<typename Handler>
//   void get_content(const std::string & url, Handler handler)
//   {
//      content_.clear();
//      io_service_.post(
//         boost::bind(&downloader::init<boost::tuple<Handler> >, this, url,
//            boost::make_tuple(handler)));
//   }
//
//};
//
//class uploader
//{
//
//};

} //namespace curl
