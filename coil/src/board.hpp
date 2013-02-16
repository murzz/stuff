#pragma once

#include <string>
#include <vector>

struct board
{
    size_t x_;
    size_t y_;

    typedef enum square_type_tag
    {
        empty = '.',
        wall = 'X'
    } square_type;
    typedef std::vector<square_type> type;
    type board_;
    std::string board_str_;
};
