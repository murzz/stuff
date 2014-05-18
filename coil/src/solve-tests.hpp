#pragma once

#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>

#define UNIT_TEST_BUILD
#include "solve.hpp"

BOOST_AUTO_TEST_CASE( solve_test )
{
   const std::size_t width = 5, height = 3;
   const std::string cells = "......X......X.";
   coil::board board(width, height, cells);

   boost::asio::io_service io_service;
   io_service.post(boost::bind(solve, boost::ref(io_service), board));
   io_service.run();
}
