#include "puzzle.hpp"
#include "symbol.hpp"
#include <assert.h>
#include <bit>
#include <bitset>
#include <tuple>
#include <vector>

/**
 * Solves the puzzle using backtracking.
 */
bool Puzzle::backtracking()
{
    int row;
    int col;
    int ox;
    int oy;
    int popped_i;
    int popped_j;
    char popped_symbol = symbol::unassigned_symbol;
    bool failure;
    int total_guesses = 0;
    std::vector<std::tuple<size_t, size_t, char>> stack;

    calculate_all_candidates();

    // find next unassigned cell
find_first_unassigned_cell:;

    for (row = 0; row < gridSize; row++)
    {
        for (col = 0; col < gridSize; col++)
        {
            if (m_board[row][col] == symbol::unassigned_symbol)
            {
                goto found_first_unassigned_cell;
            }
        }
    }
    // If no unassigned cells are found, means that the puzzle is solved.
    return true;
found_first_unassigned_cell:;

    // if we just failed, then we popped a cell and unassigned it.
    // that cell should always be the first unassigned cell.
    if (failure)
    {
        assert(popped_i == row && popped_j == col);
    }

    char symbol = symbol::get_next_symbol_from_mask(m_candidates[row][col], popped_symbol);
    // char symbol = get_first_symbol_from_mask(m_candidates[row][col]);
    uint16_t symbol_mask = symbol::get_symbol_mask(symbol);

    if (m_candidates[row][col] == 0 || symbol == symbol::unassigned_symbol)
    {
        goto failure_label;
    }

    m_board[row][col] = symbol;
    total_guesses++;

    stack.push_back(std::tuple(row, col, symbol));
    failure = false;
    popped_symbol = symbol::unassigned_symbol;

    // - update backtracking candidates with info that we are removing this candidate
    // - remove the just-assigned symbol from candidates of unassigned neighbors
    // - right after removing them, perform the check to make sure there are still candidates

    // row
    for (int j = 0; j < gridSize; j++)
    {
        if (m_board[row][j] == symbol::unassigned_symbol && (m_candidates[row][j] & symbol_mask))
        {
            m_backtrack_candidates_removed[row][col].insert((row * gridSize) + j);

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
        if (m_board[i][col] == symbol::unassigned_symbol && (m_candidates[i][col] & symbol_mask))
        {
            m_backtrack_candidates_removed[row][col].insert((i * gridSize) + col);
            m_candidates[i][col] &= ~symbol_mask;

            if (m_candidates[i][col] == 0)
            {
                failure = true;
            }
        }
    }

    // square
    ox = (row / squareSize) * squareSize;
    oy = (col / squareSize) * squareSize;
    for (int i = ox; i < ox + squareSize; i++)
    {
        for (int j = oy; j < oy + squareSize; j++)
        {
            if (m_board[i][j] == symbol::unassigned_symbol && (m_candidates[i][j] & symbol_mask))
            {
                m_backtrack_candidates_removed[row][col].insert((i * gridSize) + j);
                m_candidates[i][j] &= ~symbol_mask;

                if (m_candidates[i][j] == 0)
                {
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
                if (m_board[i][j] == symbol::unassigned_symbol && m_candidates[i][j] == 0)
                {
                    failure = true;
                }
            }
        }
    }

    if (failure)
    {
    failure_label:;
        if (!stack.size())
        {
            return false;
        }
        auto tup = stack.back();

        stack.pop_back();
        popped_i = std::get<0>(tup);
        popped_j = std::get<1>(tup);
        popped_symbol = std::get<2>(tup);
        uint16_t symbol_mask = symbol::get_symbol_mask(popped_symbol);

        assert(popped_symbol != symbol::unassigned_symbol);
        assert(m_board[popped_i][popped_j] == popped_symbol);
        m_board[popped_i][popped_j] = symbol::unassigned_symbol;

        for (auto index : m_backtrack_candidates_removed[popped_i][popped_j])
        {
            int row = index / gridSize;
            int col = index % gridSize;

            // only restore the candidates if it is not the failed cell.
            // The failed cell needs to keep that candidate removed, so
            // we dont try it again, since it didnt work out.
            if (row != popped_i || col != popped_j)
            {
                m_candidates[row][col] |= symbol_mask;
            }
        }
        m_backtrack_candidates_removed[popped_i][popped_j].clear();
    }
    goto find_first_unassigned_cell;
}