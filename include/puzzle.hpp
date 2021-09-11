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

public:
    Puzzle(const char *board)
    {
        memcpy(m_board, board, sizeof(m_board));
    }

    Puzzle()
    {
        memset(m_board, 0, sizeof(m_board));
        memset(m_candidates, 0, sizeof(m_candidates));
    }

    bool calculate_candidates(uint8_t i, uint8_t j);

    void print_candidates(uint8_t i, uint8_t j);

    void print_all_candidates();

    void calculate_all_candidates();

    uint8_t assign_simple_candidates();

    size_t count_unassigned_cells();

    void find_and_assign_singular_candidates();

    void remove_symbol_from_candidates_in_constraint_zones(uint8_t i, uint8_t j, char symbol);

    void calculate_candidates_for_constraint_zone(int x, int y);

    void remove_some_candidates();

    ScientificNotation num_possible_permutations();

    // would be nice to rewrite this without gotos?
    void backprop();

    void print_board();
    uint8_t iter_solve_puzzle();
};
