//#include <iostream>
#include <cstdlib>

#include "board.hpp"
#include "parser.hpp"
#include "solver.hpp"

int main(int argc, char** argv)
{
   size_t x = 0;
   size_t y = 0;
   std::string board_str;

   try
   {
      parse(argc, argv, x, y, board_str);
   }
   catch (const board_not_parsed&)
   {
      return EXIT_FAILURE;
   }

   board board(x, y, board_str);
//std::copy(board.board_.begin(), board.board_.end(),
   //          std::ostream_iterator<board::square_type>(std::cout));

   solve(board);
   return EXIT_SUCCESS;
}
