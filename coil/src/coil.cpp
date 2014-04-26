#include <cstdlib>
#include <boost/asio.hpp>
#include "cmdline-parser.hpp"

void error_handler(const boost::system::error_code & ec)
{
   //(ec ? std::cerr : std::cout) << ec.message() << std::endl;
   if (ec)
   {
      std::cerr << ec.message() << std::endl;
   }
}

class worker
{
public:
   worker(int argc, char** argv)
   {
      typedef boost::function<void(const boost::system::error_code &)> error_handler_type;
      error_handler_type eh = boost::bind(error_handler, boost::asio::placeholders::error);
      ios.post(
         boost::bind(parse<boost::tuple<error_handler_type> >, boost::ref(ios), argc, argv,
            boost::make_tuple(eh)));
      ios.run();

   }
private:
   boost::asio::io_service ios;
   // here we can instantiate curl::downloader, board etc..
};

int main(int argc, char** argv)
{
   try
   {
      worker(argc, argv);
   }
   catch (const std::exception& e)
   {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
