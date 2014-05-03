#pragma once

//#include <algorithm>
//#include <iterator>
//#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/function.hpp>

#include "curl.hpp"

namespace test_helpers
{

template<typename Handler>
void html_handler(boost::asio::io_service & io_service, Handler handler, const std::string & html)
{
   // html is here
   const bool is_html = std::string::npos != html.find("<html>")
      || std::string::npos != html.find("<HTML>");
   BOOST_REQUIRE_EQUAL(is_html, true);

//   std::copy(html.begin(), html.end(),
//      std::ostream_iterator<std::string::value_type>(std::cout, " "));

// here html_handler supposed to call dummy_handler, if needed
}

void dummy_handler()
{
   BOOST_ERROR("Should be never called");
}

template<typename Handler>
struct templated_functor
{
   typedef boost::function<
      void(boost::asio::io_service & io_service, Handler handler, const std::string & html)> html_handler_type;
};

typedef boost::function<void()> dummy_handler_type;
dummy_handler_type dummy_handler_functor = boost::bind(dummy_handler);

typedef typename templated_functor<dummy_handler_type>::html_handler_type html_handler_type;

struct worker
{
   boost::asio::io_service io_service_;
   bool is_url_set;

   worker() :
      is_url_set(false)
   {

   }

   void set_url(const std::string & url)
   {
      is_url_set = true;

      dummy_handler_type dummy_handler_functor = boost::bind(dummy_handler);

      html_handler_type html_handler_functor = boost::bind(
         html_handler<dummy_handler_type>,
         boost::ref(io_service_),
         dummy_handler_functor,
         _3);

      io_service_.post(
         boost::bind(
            curl::download<
               boost::tuple<html_handler_type, dummy_handler_type> >,
            boost::ref(io_service_),
            boost::make_tuple(html_handler_functor, dummy_handler_functor),
            url));
   }

   void run()
   {
      BOOST_REQUIRE(is_url_set);

      std::size_t handlers_count = 0;
      BOOST_REQUIRE_NO_THROW(handlers_count = io_service_.run());
      BOOST_REQUIRE_NE(handlers_count, 0);
   }
};

} // namespace curl_test_helpers

BOOST_FIXTURE_TEST_CASE( download_html, test_helpers::worker )
{
   set_url("http://google.com");
   run();
}

BOOST_FIXTURE_TEST_CASE( download_wrong_host, test_helpers::worker )
{
   set_url("http://unresolvable-hostname-for-sure-nyash-myash.com");

   BOOST_REQUIRE_THROW(io_service_.run(), boost::system::error_code);
}

BOOST_FIXTURE_TEST_CASE( download_wrong_host_check_type, test_helpers::worker )
{
   const std::string url = "another-unresolvable-hostname-for-sure-nyash-myash.com";
   set_url(url);
   try
   {
      io_service_.run();
      BOOST_ERROR("Should throw, but it doesn't");
   }
   catch (const boost::system::error_code & ec)
   {
      boost::system::error_code no_host = curl::error::make_error_code(CURLE_COULDNT_RESOLVE_HOST);
      BOOST_REQUIRE_EQUAL(ec, no_host);
   }
}

BOOST_FIXTURE_TEST_CASE( download_no_connection, test_helpers::worker )
{
   // this test runs long time

   const std::string url = "8.8.8.8";
   set_url(url);
   try
   {
      io_service_.run();
      BOOST_ERROR("Should throw, but it doesn't");
   }
   catch (const boost::system::error_code & ec)
   {
      boost::system::error_code no_host = curl::error::make_error_code(CURLE_COULDNT_CONNECT);
      BOOST_REQUIRE_EQUAL(ec, no_host);
   }
}

