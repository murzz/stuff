#include <cstdlib>

#include <boost/asio.hpp>
//#include <boost/asio/signal_set.hpp>
#include <boost/thread.hpp>
#include <boost/log/trivial.hpp>

#include "cmdline-parser.hpp"
#include "board.hpp"
#include "solve.hpp"
#include "env.hpp"

void board_handler(boost::asio::io_service & io_service, coil::board & board)
{
   io_service.post(boost::bind(solve, boost::ref(io_service), board));
}

int main(int argc, char** argv)
{
   try
   {
      BOOST_LOG_TRIVIAL(info)<< "Started";

      env::get().argc_ = argc;
      env::get().argv_ = argv;

      boost::asio::io_service io_service;

      //TODO: run() will wait forever because of this work, need to find a way to cancel it on graceful exit (all handlers completed)
      // handle signals
//      boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
//      signals.async_wait(boost::bind(&boost::asio::io_service::stop, &io_service));

      // parse command line
      typedef boost::function<void(coil::board board)> board_handler_type;
      board_handler_type handler = boost::bind(board_handler, boost::ref(io_service), _1);

      io_service.post(
         boost::bind(cmdline::parse<boost::tuple<board_handler_type> >, boost::ref(io_service),
            boost::make_tuple(handler), argc, argv, nullptr));

      io_service.run();
      env::get().threads_.join_all();
      BOOST_LOG_TRIVIAL(info) << "Finished";
   }
   catch (const std::exception & e)
   {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
   }
   catch (const boost::system::error_code & ec)
   {
      std::cerr << ec.message() << std::endl;
      return EXIT_FAILURE;
   }
   catch (...)
   {
      std::cerr << "Unknown exception" << std::endl;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
