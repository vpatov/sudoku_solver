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

class Puzzle
{
public:
    const static int gridSize = 9;
    const static int numSymbols = 9;
    const static int squareSize = 3;
    const static size_t uint16_bits = std::numeric_limits<uint16_t>::digits;
    const static size_t uint32_bits = std::numeric_limits<uint32_t>::digits;

private:
    // m_board is the sudoku grid. unassigned cells are '0', assigned cells are '1'.
    // in retrospect, making these chars was a mistake. they should just be int8_t.
    char m_board[gridSize][gridSize] = {0};

    // m_candidates[i][j] contains a bitset. if the ith bit is set, it means that the symbol i is
    // a possible candidate for cell m_board[i][j].
    // if m_candidates[i][j] == 0 && m_board[i][j] == '0', then we have reached a conflict during backprop
    uint16_t m_candidates[gridSize][gridSize] = {0};

    // m_backprop_candidates[i][j] contains the a bitset, with the ith bit
    // representing the ith index as having had the symbol s (from m_board[i][j]) removed from the
    // candidates at m_candidates[i][j]
    // indeces 0..8 for the row, 9..17 for the col, 18..27 for the 3x3 square
    // std::bitset<uint32_bits> m_backprop_candidates2[gridSize][gridSize] = {0};

    // std::bitset<gridSize * gridSize> m_backprop_candidates3[gridSize][gridSize] = {0};

    std::unordered_set<size_t> m_backprop_candidates[gridSize][gridSize];

    const static std::string borderColor;
    const static std::string symbolColor;

    // maps numbers to the cells that they could possibly be in
    std::unordered_set<size_t> candidate_cells[numSymbols];

    // numbers_in_rows[0] = contains the set of numbers present in the 1st row
    std::unordered_set<char> symbols_in_rows[gridSize];
    std::unordered_set<char> symbols_in_cols[gridSize];

public:
    Puzzle(char board[gridSize][gridSize])
    {
        memcpy(m_board, board, sizeof(m_board));
    }

    Puzzle(char **board)
    {
        memcpy(m_board, board, sizeof(m_board));
    }

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

    void initialize_helper_vars();

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

    void initialize_candidate_map()
    {
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                for (char c = '1'; c <= '9'; c++)
                {
                    if (m_board[i][j] != '0')
                    {
                        continue;
                    }
                    if (symbols_in_rows[i].find(c) == symbols_in_rows[i].end() &&
                        symbols_in_cols[j].find(c) == symbols_in_cols[j].end())
                    {
                        candidate_cells[c - '1'].insert((i * gridSize) + j);
                    }
                }
            }
        }
    }

    char get_first_symbol_from_mask(uint16_t candidate_mask)
    {
        return '0' + (uint16_bits - std::__countl_zero(candidate_mask));
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
                    char symbol = '0' + (uint16_bits - std::__countl_zero(m_candidates[i][j]));
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

    uint16_t get_symbol_mask(char symbol)
    {
        return 1UL << (symbol - '1');
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

                    if (m_candidates[i][j] & get_symbol_mask(symbol))
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
                    if (m_candidates[i][j] & get_symbol_mask(symbol))
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

                        if (m_candidates[i][j] & get_symbol_mask(symbol))
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
        // 6, 0 -> 6
        // 8, 3 -> 7
        // 4, 5 -> 4
        // uint16_t offset = ((x / squareSize) * squareSize) + (y / 3);

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
                uint16_t symbol_mask = get_symbol_mask(symbol);
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

    void print_all_candidates()
    {
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                std::cout << "(" << i << ", " << j << "): ";
                for (int s = 0; s < 9; s++)
                {
                    if ((1UL << s) & m_candidates[i][j])
                    {
                        std::cout << (s + 1) << " ";
                    }
                }
                std::cout << std::endl;
            }
        }
    }

    // find first empty cell, try taking one of candidates, and assigning it.
    // keep track of how you are changing the state during assignment of
    // m_board
    // m_candidates

    void forward_state(int i, int j, char symbol)
    {
    }

    // Idea: candidates for a cell only ever decrease in cardinality as you move closer towards in solution
    // Therefore, a possible optimization for calculate_all_candidates would be to only iterate over
    // the symbols that are in the existing candidates, and see which ones need to be removed because
    // they are no longer legal (after new assignments are tried).
    // Question: should the whole candidates array be iterated over with this rule? i.e.
    // for row in candidates:
    //      for candidate in row:
    //          if candidate != 0:
    //              for set_bit in candidate:
    //                  # check row, col, and square for candidate
    // or, is it sufficient to only iterate

    // takes the first possible entry for m_board[i][j] from candidates and assigns it,
    // and removes it from the candidates.
    // Do other candidates need to be updated? (yes)
    bool try_assign(int i, int j)
    {
        // uint16_t candidate_bits = m_candidates[i][j];
        // while (candidate_bits)
        // {
        //     char symbol = get_first_symbol_from_mask(candidate_bits);
        //     candidate_bits &= ~get_symbol_mask(symbol);
        //     try_assign(i, j)
        // }

        char symbol = get_first_symbol_from_mask(m_candidates[i][j]);
        m_candidates[i][j] &= ~get_symbol_mask(symbol); // isn't necessary if we call calculate_all_candidates
        m_board[i][j] = symbol;
        calculate_candidates_for_constraint_zone(i, j);
        calculate_all_candidates();
        return symbol;
    }

    void undo_assign(int i, int j)
    {
        // m_board[i][j] = 0;
        // m_candidates[i][j] &= get_symbol_mask(symbol);
        // calculate_all_candidates();
    }

    // need to do this not recursively
    void backprop()

    {
        std::vector<std::tuple<size_t, size_t, char>> stack;

        // find next unassigned cell
        int row = 0;
        int col = 0;
        for (; row < gridSize; row++)
        {
            for (; col < gridSize; col++)
            {
                if (m_board[row][col] == '0')
                {
                    goto found_first_unassigned_cell;
                }
            }
        }
    found_first_unassigned_cell:;

        char symbol = get_first_symbol_from_mask(m_candidates[row][col]);
        uint16_t symbol_mask = get_symbol_mask(symbol);
        stack.push_back(std::tuple(row, col, symbol));

        bool failure = false;

        // - update backprop candidates with info that we are removing this candidate
        // - remove the just-assigned symbol from candidates of unassigned neighbors
        // - right after removing them, perform the check to make sure there are still candidates

        // row
        for (int j = 0; j < gridSize; j++)
        {
            if (m_candidates[row][j] & symbol_mask)
            {
                m_backprop_candidates[row][j].insert(row * gridSize + j);
                m_candidates[row][j] &= ~symbol_mask;

                if (m_candidates[row][j] == 0)
                {
                    failure = true;
                }
            }
        }
        // col
        for (int i = 0; i < gridSize; i++)
        {
            if (m_candidates[i][col] & symbol_mask)
            {
                m_backprop_candidates[i][col].insert(i * gridSize + col);
                m_candidates[i][col] &= ~symbol_mask;

                if (m_candidates[i][col] == 0)
                {
                    failure = true;
                }
            }
        }

        // 3x3 square
        int ox = (row / squareSize) * squareSize;
        int oy = (col / squareSize) * squareSize;
        for (int i = ox; i < ox + squareSize; i++)
        {
            for (int j = oy; j < oy + squareSize; j++)
            {
                if (m_candidates[i][j] & symbol_mask)
                {
                    m_backprop_candidates[i][j].insert((i * gridSize) + j);
                    m_candidates[i][j] &= ~symbol_mask;

                    if (m_candidates[i][j] == 0)
                    {
                        failure = true;
                    }
                }
            }
        }

        if (failure)
        {
            // pop from stack to get row, col, symbol
            // assert that m_board[row][col] == symbol
            // unassign symbol from m_board[row][col]
            // look at m_backprop[row][col] to see which cells need to have symbol re-added to their candidates
            // for set_bit in m_backprop[row][col]:
            //      cell = get_cell_from_m_backprop_bitset_bit(set_bit)
            //
            auto tup = stack.back();
            stack.pop_back();
            int i = std::get<0>(tup);
            int j = std::get<1>(tup);
            char symbol = std::get<2>(tup);

            assert(m_board[i][j] == symbol);
            m_board[i][j] = '0';

            for (auto bit : m_backprop_candidates[i][j])
            {
            }
        }
    }

    void print_board();
    uint8_t iter_solve_puzzle();
};
