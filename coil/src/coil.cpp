#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include "cmdline-parser.hpp"
#include "board.hpp"
#include "solver.hpp"

void board_handler(boost::asio::io_service & io_service, const coil::board & board)
{
   io_service.post(boost::bind(solve, boost::ref(io_service), board));
}

int main(int argc, char** argv)
{
   try
   {
      boost::asio::io_service io_service;

      //TODO: run() will wait forever because of this work, need to find a way to cancel it on graceful exit (all handlers completed)
      // handle signals
//      boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
//      signals.async_wait(boost::bind(&boost::asio::io_service::stop, &io_service));

      // parse command line
      typedef boost::function<void(coil::board board)> handler_type;
      handler_type handler = boost::bind(board_handler, boost::ref(io_service), _1);

      io_service.post(
         boost::bind(cmdline::parse<boost::tuple<handler_type> >, boost::ref(io_service),
            boost::make_tuple(handler), argc, argv, nullptr));

      // do the job
      std::size_t handlers_count = io_service.run();
      std::cout << handlers_count << " handler(s) were executed" << std::endl;
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
   return EXIT_SUCCESS;
}
