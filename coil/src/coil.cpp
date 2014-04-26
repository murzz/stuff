#include <cstdlib>
#include <boost/asio.hpp>
#include "cmdline-parser.hpp"

void error_handler(const boost::system::error_code & ec)
{
   (ec ? std::cerr : std::cout) << ec.message() << std::endl;
}

int main(int argc, char** argv)
{
   boost::asio::io_service ios;

   typedef boost::function<void(const boost::system::error_code &)> error_handler_type;
   error_handler_type eh = boost::bind(error_handler, boost::asio::placeholders::error);
   ios.post(
      boost::bind(parse<boost::tuple<error_handler_type> >, argc, argv, boost::make_tuple(eh)));

//   ios.post(
//      boost::bind(parse<boost::tuple<boost::function<void(const boost::system::error_code &)>>>,
//   argc, argv,
//   boost::make_tuple(boost::bind(error_handler, boost::asio::placeholders::error))));

   ios.run();
   return EXIT_SUCCESS;
}
