#pragma once

//#define BOOST_TEST_MODULE board tests
#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio/placeholders.hpp>

#include "downloader.hpp"

void html_handler(const boost::system::error_code & ec, const std::string & html)
{
   if (ec)
   {
      //if (downloader::get_error_category() == ec.category())
      {
         BOOST_TEST_MESSAGE(ec.message());
         std::cout << "error: " << ec.message() << std::endl;
      }
   }
   std::cout << "html: " << html << std::endl;
   // no errors
   boost::system::error_code no_error;
   BOOST_REQUIRE_EQUAL(ec, no_error);

   // html is here
   const bool is_html = std::string::npos != html.find("<html>")
      || std::string::npos != html.find("<HTML>");
   BOOST_REQUIRE_EQUAL(is_html, true);
}

BOOST_AUTO_TEST_CASE( downloader_test1 )
{
   boost::asio::io_service io_service;
   //boost::asio::io_service::work work(io_service);
   downloader downloader(io_service);
   const std::string url = "http://googssle.com";
   downloader.get_html(url, boost::bind(html_handler, _1, _2));

   io_service.run();
}
