#include "colors.hpp"
#include "puzzle.hpp"
#include <iostream>
#include <memory>

const char *puzzle1_str =
    "904200001200080009600000007000000400000039018100008000378540002420006000060892300";

int main()
{
    std::cout << Color::green << "Sudoku Solver 1.0" << Color::endl;
    std::cout << Color::purple << "By Vasia Patov" << Color::endl;
    std::cout << Color::blue << "Solving..." << Color::endl;

    Puzzle puzzle(puzzle1_str);
    puzzle.print_board();

    puzzle.initialize_helper_vars();
}