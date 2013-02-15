//#include <iostream>
#include <cstdlib>

#include "params.hpp"

int main(int argc, char** argv)
{

    board board;
    parse(board, argc, argv);

    std::copy(board.board_.begin(), board.board_.end(),
                std::ostream_iterator<board::square_type>(std::cout));

    return EXIT_SUCCESS;
}
