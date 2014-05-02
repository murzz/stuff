#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include "cmdline-parser.hpp"
#include "board.hpp"

//class worker
//{
//public:
//   worker(int argc, char** argv)
//   {
////      typedef boost::function<void(const boost::system::error_code &)> error_handler_type;
////      error_handler_type eh = boost::bind(&worker::error_handler, this,
////         boost::asio::placeholders::error);
////      ios.post(
////         boost::bind(parse<boost::tuple<error_handler_type> >, boost::ref(ios), argc, argv,
////            boost::make_tuple(eh)));
//      ios.post(boost::bind(parse, boost::ref(ios), argc, argv));
//      ios.run();
//   }
//
//private:
//   boost::asio::io_service ios;
//   // here we can instantiate curl::downloader, board etc..
//
//   void error_handler(const boost::system::error_code & ec)
//   {
//      //(ec ? std::cerr : std::cout) << ec.message() << std::endl;
//      if (ec)
//      {
//         std::cerr << ec.message() << std::endl;
//      }
//   }
//};

void board_handler(coil::board board)
{
   std::cout << __func__ << std::endl;
// solve it here
}

int main(int argc, char** argv)
{
   try
   {
      boost::asio::io_service io_service;

      //TODO: run() will wait forever because of this work, need to find a way to cancel it on graceful exit (all handlers completed)
      // handle signals
      //boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
      //signals.async_wait(boost::bind(&boost::asio::io_service::stop, &io_service));

      // parse command line
      typedef boost::function<void(coil::board board)> handler_type;
      handler_type handler = boost::bind(board_handler, _1);

      io_service.post(
         boost::bind(cmdline::parse<boost::tuple<handler_type> >, boost::ref(io_service),
            boost::make_tuple(handler), argc, argv, nullptr));

      // do the job
      std::size_t handlers_count = io_service.run();
      std::cout << handlers_count << " handler(s) were executed" << std::endl;
   }
   catch (const std::exception& e)
   {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
