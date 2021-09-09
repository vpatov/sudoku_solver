#include "colors.hpp"
#include "puzzle.hpp"
#include <iostream>
#include <memory>
#include <bitset>

const char *puzzle1_str =
    "904200001200080009600000007000000400000039018100008000378540002420006000060892300";

const char *puzzle2_str =
    "791254680240687905800913020430162098912538760586409132304891256158020340029045071";

const char *puzzle3_str =
    "090140000005000020030000060046000000120930045003004006400001200080400003350700900";
const char *puzzle4_str =
    "063000008908500000000070000000004500830050090009610000050030460000000000200100030";

void foo()
{
    int a = std::__countl_zero((uint16_t)0b11001001100);
    int b = std::__countl_zero((uint32_t)0b01100100);
    int c = std::__countl_zero((uint16_t)0b101111000100000);
    int d = std::__countl_zero((uint32_t)0b1101100000);
    int e = std::__countl_zero((uint16_t)0b1001100100000);
    int f = std::__countl_zero((uint32_t)0b00110000);
    int g = std::__countl_zero((uint16_t)0b01100000);
    int h = std::__countl_zero((uint32_t)0b100101100000);
    int i = std::__countl_zero((uint16_t)0b10100000);
    int j = std::__countl_zero((uint32_t)0b01100100);
    int k = std::__countl_zero((uint16_t)0b1100001110100000);
    int l = std::__countl_zero((uint32_t)0b1100101100000);

    int zzz = a + b + c + d + e + f + g + h + i + j + k + l;
    std::cout << zzz << std::endl;
}

int main()
{
    // foo();

    std::cout << Color::green << "Sudoku Solver 1.0" << Color::endl;
    std::cout << Color::purple << "By Vasia Patov" << Color::endl;
    std::cout << Color::blue << "Solving..." << Color::endl;

    Puzzle puzzle(puzzle3_str);

    // puzzle.print_board();

    // puzzle.initialize_helper_vars();
    // puzzle.initialize_candidate_map();

    /*
    puzzle.print_board();

    int prev_unassigned_cells = Puzzle::gridSize * Puzzle::gridSize;
    while (true)

    {
        uint8_t unassigned_cells = puzzle.iter_solve_puzzle();
        if (unassigned_cells == 0)
        {
            std::cout << Color::green << "Solved!!!" << Color::endl;
            break;
        }
        if (unassigned_cells == prev_unassigned_cells)
        {
            std::cout << Color::red << "logic implemented is not enough - need to try backprop..." << Color::endl;
            break;
        }
        prev_unassigned_cells = unassigned_cells;
        std::cout << (uint16_t)unassigned_cells << std::endl;
    }

    puzzle.calculate_all_candidates();

    puzzle.print_board();
    */
}