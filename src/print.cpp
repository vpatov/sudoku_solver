#include "print.hpp"
#include <iostream>
#include "colors.hpp"

void print_dashes(std::string color, int count)
{
    std::cout << color;
    for (int i = 0; i < count; i++)
    {
        std::cout << '-';
    }
    std::cout << Color::endl;
}

void newline()
{
    std::cout << std::endl;
}