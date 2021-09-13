#include "process_args.hpp"
#include "colors.hpp"
#include "puzzle.hpp"
#include <iostream>

void print_header()
{
    std::cout << Color::green << "Sudoku Solver 1.0" << Color::endl;
    std::cout << Color::purple << "By Vasia Patov" << Color::endl
              << std::endl;
}

int main(int argc, char *argv[])
{
    print_header();
    parse_args(argc, argv);
    process_args();
}