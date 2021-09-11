#include "symbol.hpp"
#include "puzzle.hpp"

/**
 * Returns the number of cells that do not have a symbol assigned to them yet.
 */
size_t Puzzle::count_unassigned_cells()
{
    size_t unassigned_cells = 0;
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            if (m_board[i][j] == symbol::unassigned_symbol)
            {
                unassigned_cells++;
            }
        }
    }
    return unassigned_cells;
}

/**
 * Calculate the total amount of possible assignments (legal and illegal) for a given puzzle.
 * Equivalent to the product of the number of candidates per cell.
*/
ScientificNotation Puzzle::num_possible_permutations()
{
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