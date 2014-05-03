#pragma once

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

   // here html_handler supposed to call dummy_handler, if needed
}

void dummy_handler()
{

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

} // namespace curl_test_helpers

BOOST_AUTO_TEST_CASE( download_html )
{
   boost::asio::io_service io_service;

   const std::string url = "http://google.com";

   //TODO make it simpler

   test_helpers::html_handler_type html_handler_functor = boost::bind(
      test_helpers::html_handler<test_helpers::dummy_handler_type>,
      boost::ref(io_service),
      test_helpers::dummy_handler_functor,
      _3);

   io_service.post(
      boost::bind(
         curl::download<
            boost::tuple<test_helpers::html_handler_type, test_helpers::dummy_handler_type> >,
         boost::ref(io_service),
         boost::make_tuple(html_handler_functor, test_helpers::dummy_handler_functor),
         url));

   BOOST_REQUIRE_NO_THROW(io_service.run());
}

BOOST_AUTO_TEST_CASE( download_wrong_host )
{
   boost::asio::io_service io_service;

   const std::string url = "http://unresolvable-hostname-for-sure-nyash-myash.com";

   test_helpers::html_handler_type html_handler_functor = boost::bind(
      test_helpers::html_handler<test_helpers::dummy_handler_type>,
      boost::ref(io_service),
      test_helpers::dummy_handler_functor,
      _3);

   io_service.post(
      boost::bind(
         curl::download<
            boost::tuple<test_helpers::html_handler_type, test_helpers::dummy_handler_type> >,
         boost::ref(io_service),
         boost::make_tuple(html_handler_functor, test_helpers::dummy_handler_functor),
         url));

   // TODO check particular error code
   //boost::system::error_code no_host = curl::error::make_error_code(CURLE_COULDNT_RESOLVE_HOST);
   BOOST_REQUIRE_THROW(io_service.run(), boost::system::error_code);
}
