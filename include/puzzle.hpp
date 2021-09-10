#pragma once
#include <string>
#include <iostream>
#include "colors.hpp"
#include <unordered_map>
#include <unordered_set>
#include <bit>
#include <limits>
#include <vector>
#include <tuple>
#include <bitset>
#include <assert.h>
#include "bitmask.hpp"
#include "util.hpp"

class Puzzle
{
public:
    const static int gridSize = 9;
    const static int numSymbols = 9;
    const static int squareSize = 3;

private:
    // m_board is the sudoku grid. unassigned cells are '0', assigned cells are '1'.
    // in retrospect, making these chars was a mistake. they should just be int8_t.
    char m_board[gridSize][gridSize] = {0};

    // m_candidates[i][j] contains a bitset. if the ith bit is set, it means that the symbol i is
    // a possible candidate for cell m_board[i][j].
    // if m_candidates[i][j] == 0 && m_board[i][j] == '0', then we have reached a conflict during backprop
    uint16_t m_candidates[gridSize][gridSize] = {0};

    std::unordered_set<size_t> m_backprop_candidates[gridSize][gridSize];

    const static std::string borderColor;
    const static std::string symbolColor;

public:
    Puzzle(const char *board)
    {
        memcpy(m_board, board, sizeof(m_board));
    }

    Puzzle()
    {
        memset(m_board, 0, sizeof(m_board));
    }

    void print_dashes(std::string color, int count)
    {
        using namespace std;
        cout << color;
        for (int i = 0; i < count; i++)
        {
            cout << '-';
        }
        cout << Color::endl;
    }

    bool calculate_candidates(size_t i, size_t j);

    void calculate_all_candidates()
    {
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {

                // if the cell is empty, calculate possible candidates by going through
                // the square, rows, and cols.
                calculate_candidates(i, j);
            }
        }
        remove_some_candidates();
    }

    uint8_t assign_simple_candidates()
    {
        uint8_t unassigned_cells = 0;
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                uint8_t popcount = std::__popcount(m_candidates[i][j]);
                if (popcount == 1)
                {
                    char symbol = bitmask::get_first_symbol_from_mask(m_candidates[i][j]);
                    m_board[i][j] = symbol;
                    m_candidates[i][j] = 0;
                }
                // if the popcount is not one or zero, then this means this cell has not been determined yet.
                else if (popcount != 0)
                {
                    unassigned_cells++;
                }
            }
        }
        return unassigned_cells;
    }

    size_t count_unassigned_cells()
    {
        size_t unassigned_cells = 0;
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                if (m_board[i][j] == '0')
                {
                    unassigned_cells++;
                }
            }
        }
        return unassigned_cells;
    }

    // see if in a row set, a column set, or square set, there is only cell that could have a particular symbol
    // assign it if so
    void assign_other_candidates1()
    {
        for (char symbol = '1'; symbol <= '9'; symbol++)
        {
            // check rows
            for (int i = 0; i < gridSize; i++)
            {
                int cell_count = 0;
                int candidate_column = 0;
                for (int j = 0; j < gridSize; j++)
                {

                    if (m_candidates[i][j] & bitmask::get_symbol_mask(symbol))
                    {
                        cell_count++;
                        candidate_column = j;
                    }
                }

                // if we found only one cell that can have this symbol, assign it.
                // problem with this is that, candidates are not up to date.
                if (cell_count == 1)
                {
                    m_board[i][candidate_column] = symbol;
                    m_candidates[i][candidate_column] = 0;
                }
            }
            // need to update candidates now
            calculate_all_candidates();

            // check cols
            for (int j = 0; j < gridSize; j++)
            {
                int cell_count = 0;
                int candidate_row = 0;
                for (int i = 0; i < gridSize; i++)
                {
                    if (m_candidates[i][j] & bitmask::get_symbol_mask(symbol))
                    {
                        cell_count++;
                        candidate_row = i;
                    }
                }

                // if we found only one cell that can have this symbol, assign it.
                if (cell_count == 1)
                {
                    m_board[candidate_row][j] = symbol;
                    m_candidates[candidate_row][j] = 0;
                }
            }
            calculate_all_candidates();

            //    check squares
            /*
            */
            for (int offset = 0; offset < gridSize; offset++)
            {
                int x = (offset / squareSize) * squareSize;
                int y = (offset % 3) * squareSize;

                int cell_count = 0;
                int candidate_row = 0;
                int candidate_col = 0;
                for (int i = x; i < x + squareSize; i++)
                {
                    for (int j = y; j < y + squareSize; j++)
                    {

                        if (m_candidates[i][j] & bitmask::get_symbol_mask(symbol))
                        {
                            cell_count++;
                            candidate_row = i;
                            candidate_col = j;
                        }
                    }
                }
                // if we found only one cell that can have this symbol, assign it.
                if (cell_count == 1)
                {
                    m_board[candidate_row][candidate_col] = symbol;
                    m_candidates[candidate_row][candidate_col] = 0;
                }
            }
        }
    }

    // Calculates candidates for all of the cells in the row, column, or 3x3 square
    // of cell (i,j).
    void calculate_candidates_for_constraint_zone(int x, int y)
    {
        // check row
        for (int j = 0; j < gridSize; j++)
        {
            calculate_candidates(x, j);
        }

        //check col
        for (int i = 0; i < gridSize; i++)
        {
            calculate_candidates(i, y);
        }

        //check square
        int ox = (x / squareSize) * squareSize;
        int oy = (y / squareSize) * squareSize;

        for (int i = ox; i < x + squareSize; i++)
        {
            for (int j = oy; j < oy + squareSize; j++)
            {
                calculate_candidates(i, j);
            }
        }
    }

    // for a 3x3 square, if a symbol is only a candidate for cells in one row/column, then we
    // can remove that symbol from any candidate sets for that row/column in other squares.
    void remove_some_candidates()
    {

        for (int offset = 0; offset < gridSize; offset++)
        {
            int x = (offset / squareSize) * squareSize;
            int y = (offset % 3) * squareSize;

            // you could replace this with a counter, and a row variable, similar to what you did in other methods
            std::unordered_set<size_t> rows_symbol_is_candidate_in[numSymbols];
            std::unordered_set<size_t> cols_symbol_is_candidate_in[numSymbols];

            for (char symbol = '1'; symbol <= '9'; symbol++)
            {
                size_t symbol_index = symbol - '1';
                uint16_t symbol_mask = bitmask::get_symbol_mask(symbol);
                for (int i = x; i < x + squareSize; i++)
                {
                    for (int j = y; j < y + squareSize; j++)
                    {
                        if (m_candidates[i][j] & symbol_mask)
                        {
                            rows_symbol_is_candidate_in[symbol_index].insert(i);
                            cols_symbol_is_candidate_in[symbol_index].insert(j);
                        }
                    }
                }

                // if size is 1, we can remove this symbol from cells in this row in other squares
                if (rows_symbol_is_candidate_in[symbol_index].size() == 1)
                {
                    size_t row = *rows_symbol_is_candidate_in[symbol_index].begin();
                    for (int j = 0; j < y; j++)
                    {
                        m_candidates[row][j] &= ~symbol_mask;
                    }
                    for (int j = y + squareSize; j < gridSize; j++)
                    {
                        m_candidates[row][j] &= ~symbol_mask;
                    }
                }
                if (cols_symbol_is_candidate_in[symbol_index].size() == 1)
                {
                    size_t col = *cols_symbol_is_candidate_in[symbol_index].begin();
                    for (int i = 0; i < x; i++)
                    {
                        m_candidates[i][col] &= ~symbol_mask;
                    }
                    for (int i = x + squareSize; i < gridSize; i++)
                    {
                        m_candidates[i][col] &= ~symbol_mask;
                    }
                }
            }
        }
    }

    void print_candidates(int i, int j)
    {
        for (int s = 0; s < 9; s++)
        {
            if ((1UL << s) & m_candidates[i][j])
            {
                std::cout << (s + 1) << " ";
            }
        }
        std::cout << std::endl;
    }

    void print_all_candidates()
    {
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                if (m_candidates[i][j] == 0)
                {
                    continue;
                }
                std::cout << "(" << i << ", " << j << "): ";
                print_candidates(i, j);
            }
        }
    }

    /*
        Calculate the total amount of possible assignments (legal and illegal) for a given puzzle.
        Equivalent to the product of the number of candidates per cell.
    */
    ScientificNotation num_possible_permutations()
    {
        // scientific notation
        ScientificNotation sn;

        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                int num_candidates_cell = 0;
                if (m_candidates[i][j] == 0)
                {
                    continue;
                }
                for (int s = 0; s < 9; s++)
                {
                    if ((1UL << s) & m_candidates[i][j])
                    {
                        num_candidates_cell++;
                    }
                }
                sn *= num_candidates_cell;
            }
        }
        return sn;
    }

    std::string get_product_of_candidates()
    {
        int num_non_empty_candidates = 0;
        int total_num_candidates = 0;
        std::string acc;
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                int num_candidates_cell = 0;
                if (m_candidates[i][j] == 0)
                {
                    continue;
                }
                for (int s = 0; s < 9; s++)
                {
                    if ((1UL << s) & m_candidates[i][j])
                    {
                        num_candidates_cell++;
                        total_num_candidates++;
                    }
                }
                num_non_empty_candidates++;
                acc += std::to_string(num_candidates_cell) + "*";
            }
        }
        acc += "1";

        return acc;
    }

    // would be nice to rewrite this without gotos?
    void backprop()
    {
        int row;
        int col;
        int ox;
        int oy;
        int popped_i;
        int popped_j;
        char popped_symbol = '0';
        char prev_symbol;
        bool failure;
        int num_cells_initially_unassigned = count_unassigned_cells();
        int total_guesses = 0;
        std::vector<std::tuple<size_t, size_t, char>> stack;

        std::cout
            << Color::purple << "Starting attempt at back-propogation."
            << Color::endl;
        std::cout
            << Color::yellow << "# unassigned cells: " << num_cells_initially_unassigned
            << Color::endl
            << std::endl;

        // find next unassigned cell
    find_first_unassigned_cell:;

        for (row = 0; row < gridSize; row++)
        {
            for (col = 0; col < gridSize; col++)
            {
                if (m_board[row][col] == '0')
                {
                    goto found_first_unassigned_cell;
                }
            }
        }
        std::cout
            << Color::green << "BACKPROP SUCCESS!!"
            << Color::endl;
        print_board();
        std::cout
            << Color::green << "Required a total of " << total_guesses << " guesses for "
            << num_cells_initially_unassigned << " unassigned cells."
            << Color::endl;
        return;
    found_first_unassigned_cell:;

        // if we just failed, then we popped a cell and unassigned it.
        // that cell should always be the first unassigned cell.
        if (failure)
        {
            assert(popped_i == row && popped_j == col);
        }
        char symbol = bitmask::get_next_symbol_from_mask(m_candidates[row][col], popped_symbol);
        // char symbol = get_first_symbol_from_mask(m_candidates[row][col]);
        uint16_t symbol_mask = bitmask::get_symbol_mask(symbol);

        if (m_candidates[row][col] == 0 || symbol == '0')
        {
            std::cout
                << std::endl
                << Color::purple
                << "Going to failure because first unassigned cell at "
                << row << ", " << col << " has no candidates."
                << Color::endl;
            goto failure_label;
        }

        m_board[row][col] = symbol;
        total_guesses++;

        // problem is with this line. when we make an assignment, we permanently remove that candidate
        // from the set for that cell. However, when backprop fails, and we pop up to a previous config,
        // we may well have to try the same candidate again for this cell.
        // i.e.
        // 1, (3|5), (5|7)
        // 1, 5, 7 -> fail
        // 1, 3, !! cant assign 7 because it was removed from candidates
        // removing the candidate was convenient because it made iterating over the candidates easier.
        // this problem can be solved with another matrix that keeps track
        // m_candidates[row][col] &= ~symbol_mask;

        if (row > 9 || col > 9 || symbol == '0')
        {
            std::cout << "breakpoint" << std::endl;
        }
        stack.push_back(std::tuple(row, col, symbol));
        std::cout << std::endl
                  << Color::yellow << "(" << stack.size() << ") pushing: " << row << ", " << col << ", " << symbol << Color::endl;
        // std::cout << "1,3 candidates: ";
        // print_candidates(1, 3);
        failure = false;
        popped_symbol = '0';

        // - update backprop candidates with info that we are removing this candidate
        // - remove the just-assigned symbol from candidates of unassigned neighbors
        // - right after removing them, perform the check to make sure there are still candidates

        // row
        for (int j = 0; j < gridSize; j++)
        {
            if (m_board[row][j] == '0' && (m_candidates[row][j] & symbol_mask))
            {
                std::cout << "Removing candidate "
                          << symbol << " from " << row << ", " << j << std::endl;
                std::cout << "Inserting "
                          << (row * gridSize) + j << " into m_backprop at "
                          << row << ", " << col << std::endl;
                m_backprop_candidates[row][col].insert((row * gridSize) + j);

                m_candidates[row][j] &= ~symbol_mask;

                if (m_candidates[row][j] == 0)
                {
                    std::cout
                        << Color::teal << "Encountered failure at " << row << ", " << j
                        << Color::endl;

                    failure = true;
                }
            }
        }
        // col
        for (int i = 0; i < gridSize; i++)
        {
            if (m_board[i][col] == '0' && (m_candidates[i][col] & symbol_mask))
            {
                std::cout << "Removing candidate " << symbol << " from " << i << ", " << col << std::endl;
                std::cout << "Inserting "
                          << (i * gridSize) + col << " into m_backprop at "
                          << row << ", " << col << std::endl;
                m_backprop_candidates[row][col].insert((i * gridSize) + col);
                m_candidates[i][col] &= ~symbol_mask;

                if (m_candidates[i][col] == 0)
                {
                    std::cout << Color::teal << "Encountered failure at " << i << ", " << col << Color::endl;
                    failure = true;
                }
            }
        }

        // 3x3 square
        ox = (row / squareSize) * squareSize;
        oy = (col / squareSize) * squareSize;
        for (int i = ox; i < ox + squareSize; i++)
        {
            for (int j = oy; j < oy + squareSize; j++)
            {
                if (m_board[i][j] == '0' && (m_candidates[i][j] & symbol_mask))
                {
                    m_backprop_candidates[row][col].insert((i * gridSize) + j);
                    std::cout << "Removing candidate " << symbol << " from " << i << ", " << j << std::endl;
                    std::cout << "Inserting "
                              << (i * gridSize) + j << " into m_backprop at "
                              << row << ", " << col << std::endl;
                    m_candidates[i][j] &= ~symbol_mask;

                    if (m_candidates[i][j] == 0)
                    {
                        std::cout << Color::teal << "Encountered failure at " << i << ", " << j << Color::endl;
                        failure = true;
                    }
                }
            }
        }

        // debugging sanity check for failure
        if (!failure)
        {
            for (int i = 0; i < gridSize; i++)
            {
                for (int j = 0; j < gridSize; j++)
                {
                    if (m_board[i][j] == '0' && m_candidates[i][j] == 0)
                    {
                        failure = true;
                        std::cout << Color::white
                                  << "relying on sanity check to set failure at " << i << ", " << j
                                  << Color::endl;
                    }
                }
            }
        }

        if (failure)
        {
        failure_label:;

            std::cout << Color::red << std::endl
                      << "failure. stack size: " << stack.size() << Color::endl;
            assert(stack.size());
            auto tup = stack.back();

            stack.pop_back();
            popped_i = std::get<0>(tup);
            popped_j = std::get<1>(tup);
            popped_symbol = std::get<2>(tup);
            uint16_t symbol_mask = bitmask::get_symbol_mask(popped_symbol);
            std::cout << "popped: " << popped_i << ", " << popped_j << ", " << popped_symbol << std::endl;

            if (popped_i == 1 && popped_j == 3)
            {
                std::cout << "breakpoint" << std::endl;
            }
            assert(popped_symbol != '0');
            assert(m_board[popped_i][popped_j] == popped_symbol);
            m_board[popped_i][popped_j] = '0';

            for (auto index : m_backprop_candidates[popped_i][popped_j])
            {
                int row = index / gridSize;
                int col = index % gridSize;

                // only restore the candidates if it is not the failed cell.
                // The failed cell needs to keep that candidate removed, so
                // we dont try it again, since it didnt work out.
                if (row != popped_i || col != popped_j)
                {
                    std::cout << "Restoring " << popped_symbol << " to candidates of " << row << ", " << col << std::endl;
                    m_candidates[row][col] |= symbol_mask;
                }
            }
            m_backprop_candidates[popped_i][popped_j].clear();
        }
        goto find_first_unassigned_cell;
    }

    void print_board();
    uint8_t iter_solve_puzzle();
};
