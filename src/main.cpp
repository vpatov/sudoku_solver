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

const char *almost_solved_str =
    "084007000257183649613000827000715496745000208196428700000541962420000080561892074";

const char *puzzle5_str =
    "002030008000008000031020000060050270010000050204060031000080605000000013005310400";

const char *arto_inkala_puzzle =
    "005300000800000020070010500400005300010070006003200080060500009004000030000009700";

void try_to_solve_logically(Puzzle *puzzle)
{
    int prev_unassigned_cells = Puzzle::gridSize * Puzzle::gridSize;
    while (true)

    {
        uint8_t unassigned_cells = puzzle->iter_solve_puzzle();
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
    }
}

int main()
{

    // try to force compiler warning
    // int foo[30];
    // char boo = 5;
    // std::cout << foo[boo] << std::endl;
    ///

    std::cout << Color::green << "Sudoku Solver 1.0" << Color::endl;
    std::cout << Color::purple << "By Vasia Patov" << Color::endl;
    std::cout << Color::blue << "Solving..." << Color::endl;

    Puzzle puzzle1(puzzle1_str);
    try_to_solve_logically(&puzzle1);

    Puzzle puzzle2(puzzle2_str);
    try_to_solve_logically(&puzzle2);

    Puzzle puzzle3(puzzle3_str);
    try_to_solve_logically(&puzzle3);

    // puzzle.print_board();
    // puzzle.calculate_all_candidates();
    // puzzle.print_all_candidates();

    // std::cout << puzzle.get_product_of_candidates() << std::endl;
    // ScientificNotation num_possible_permutations = puzzle.num_possible_permutations();

    // puzzle.backprop();

    // std::cout << num_possible_permutations << std::endl;
}