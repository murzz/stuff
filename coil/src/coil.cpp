//#include <iostream>
#include <cstdlib>

#include "board.hpp"

int main(int argc, char** argv)
{
    board board;

    try
    {
        parse(board, argc, argv);
    }
    catch (const exit_with_success&)
    {
        return EXIT_SUCCESS;
    }

//std::copy(board.board_.begin(), board.board_.end(),
  //          std::ostream_iterator<board::square_type>(std::cout));

    return EXIT_SUCCESS;
}
