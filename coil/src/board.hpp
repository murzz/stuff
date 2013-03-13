#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

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
    std::string str_;

    board(const size_t& x, const size_t& y, const std::string& str)
    {
        // check board size
        if (x * y != str.size())
        {
            throw std::runtime_error("board size mismatch");
        }

        //convert board from str to enum

        //std::for_each(str.begin(), str.end(), [](const std::string::value_type& square_str)
        for (const std::string::value_type& square_str : str)
        {
            board::square_type square =
                    static_cast<board::square_type>(square_str);
            switch (square)
            {
            case board::empty:
                case board::wall:
                board_.push_back(square);
                break;
            default:
                throw std::runtime_error("wrong square");
            }
        }
        //);

        x_ = x;
        y_ = y;
        str_ = str;
    }

//    board& operator=(const board& rhs)
//    {
//        if (this == &rhs)
//        {
//            return *this;
//        }
//        x_ = rhs.x_;
////TODO: so on, and do not throw
//        return *this;
//    }

//    bool operator==(const board& rhs)
//    {
//        return true;
//    }

//    friend std::ostream& operator<<(std::ostream& stream, const board& rhs)
//    {
//        return stream;
//    }

};
