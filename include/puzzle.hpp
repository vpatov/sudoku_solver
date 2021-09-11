#pragma once

#include "util.hpp"
#include <string>
#include <unordered_set>

class Puzzle
{
public:
    const static int gridSize = 9;
    const static int numSymbols = 9;
    const static int squareSize = 3;

private:
    // m_board is the sudoku grid. unassigned cells are '0', assigned cells are '1'-'9'.
    // in retrospect, making these chars was a mistake. they should just be int8_t.
    char m_board[gridSize][gridSize] = {};

    // m_candidates[i][j] contains a bitset. if the ith bit is set, it means that the symbol i is
    // a possible candidate for cell m_board[i][j].
    // if m_candidates[i][j] == 0 && m_board[i][j] == '0', then we have reached a conflict during backtracking
    uint16_t m_candidates[gridSize][gridSize] = {};

    // m_backtrack_candidates_removed[i][j] contains the set of cells (flat index representation)
    // that had the symbol currently at m_board[i][j] removed from their
    // candidate set at m_candidates[cell].
    // This is necessary so that backtracking can fully undo state changes after a path fails,
    // while trying to do as little work as possible.
    std::unordered_set<size_t> m_backtrack_candidates_removed[gridSize][gridSize] = {};

public:
    Puzzle(const char *board)
    {
        memcpy(m_board, board, sizeof(m_board));
        memset(m_candidates, 0, sizeof(m_candidates));
    }

    Puzzle()
    {
        memset(m_board, 0, sizeof(m_board));
        memset(m_candidates, 0, sizeof(m_candidates));
    }

public:
    void print_board();
    ScientificNotation num_possible_permutations();

private:
    void print_candidates(uint8_t i, uint8_t j);
    void print_all_candidates();
    size_t count_unassigned_cells();

    // NOTE: All of the functions below this line have side effects.
    // Functions for candidate calculation and pruning.
    void calculate_candidates(uint8_t i, uint8_t j);
    void calculate_candidates_for_constraint_zone(int x, int y);
    void calculate_all_candidates();
    void remove_symbol_from_candidates_in_constraint_zones(uint8_t i, uint8_t j, char symbol);
    void narrow_down_candidates();

    // Functions that perform symbol assignment based on candidate sets.
    void assign_simple_candidates();
    void find_and_assign_exclusive_candidates();

    // Function to be called in a loop to solve the puzzle using logic rules.
    uint8_t apply_logic_rules();

public:
    // Tries to use logic rules to solve the puzzle, returns true if solved,
    // false if no more progress can be made.
    bool try_to_solve_logically();

    // Catch-all function that solves any puzzle using good ol' backtracking.
    bool backtracking();
};
