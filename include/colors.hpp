#pragma once
#include <string>
#include <ostream>

namespace Color
{
    static const std::string white = "\u001b[37m";
    static const std::string teal = "\u001b[36m";
    static const std::string purple = "\u001b[35m";
    static const std::string blue = "\u001b[34m";
    static const std::string yellow = "\u001b[33m";
    static const std::string green = "\u001b[32m";
    static const std::string red = "\u001b[31m";
    static const std::string end = "\u001b[0m";

    template <class CharT, class Traits>
    std::basic_ostream<CharT, Traits> &endl(std::basic_ostream<CharT, Traits> &os)
    {
        return os << Color::end << std::endl;
    }
};
