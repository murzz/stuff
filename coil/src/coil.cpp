#include <cstdlib>

#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/thread.hpp>

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

      // create thread pool and do the job
      unsigned pool_size =
         boost::thread::hardware_concurrency() ? boost::thread::hardware_concurrency() : 1;

      boost::thread_group threads;
      for (std::size_t idx = 0; idx < pool_size - 1; ++idx)
      {
         ///@TODO to get number of handlers for each thread need to bind wrapper
         threads.create_thread(boost::bind(&boost::asio::io_service::run, boost::ref(io_service)));
      }

      std::size_t handlers_count = io_service.run();
      threads.join_all();
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
