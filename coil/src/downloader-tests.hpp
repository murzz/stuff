#pragma once

#include <boost/test/unit_test.hpp>

#include "downloader.hpp"

void ok_handler(const boost::system::error_code & ec, const std::string & html)
{
   // no errors
   boost::system::error_code no_error;
   BOOST_REQUIRE_MESSAGE(ec == no_error, ec.message());

   // html is here
   const bool is_html = std::string::npos != html.find("<html>")
      || std::string::npos != html.find("<HTML>");
   BOOST_REQUIRE_EQUAL(is_html, true);
}

BOOST_AUTO_TEST_CASE( download_html )
{
   boost::asio::io_service io_service;
   //boost::asio::io_service::work work(io_service);
   downloader downloader(io_service);
   const std::string url = "http://google.com";
   downloader.get_content(url, boost::bind(ok_handler, _1, _2));

   BOOST_REQUIRE_NO_THROW(io_service.run());
}

void wrong_hostname_handler(const boost::system::error_code & ec, const std::string & html)
{
   boost::system::error_code no_host = downloader::make_error_code(CURLE_COULDNT_RESOLVE_HOST);
   BOOST_REQUIRE_EQUAL(ec, no_host);
}

BOOST_AUTO_TEST_CASE( download_wrong_host )
{
   boost::asio::io_service io_service;
   downloader downloader(io_service);
   const std::string url = "http://unresolvable-hostname-for-sure-nyash-myash.com";
   downloader.get_content(url, boost::bind(wrong_hostname_handler, _1, _2));

   BOOST_REQUIRE_NO_THROW(io_service.run());
}
