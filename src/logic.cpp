#include "puzzle.hpp"
#include "symbol.hpp"

/**
 * Calls apply_logic_rules in a loop, and checks the amount of unassigned cells remaining  
 * after each iteration. If the amount is found to no longer be changing, then the
 * logic rules implemented (thus far) are insufficient to solve the current puzzle.
 * If there are no unassigned cells left, the puzzle is solved. 
 * 
 * @returns true when the puzzle is solved, false when it cannot be solved using 
 * the currently implemented logic rules.
 */
bool Puzzle::try_to_solve_logically()
{
    int unassigned_cells;
    int prev_unassigned_cells = gridSize * gridSize;
    while (true)
    {
        unassigned_cells = apply_logic_rules();
        if (unassigned_cells == 0)
        {
            return true;
        }
        if (unassigned_cells == prev_unassigned_cells)
        {
            return false;
        }
        prev_unassigned_cells = unassigned_cells;
    }
}

/**
 * Calculates candidates for cells, and assigns symbols to cells where it is 
 * possible to logically determine a single candidate. Returns the amount of cells
 * that are left unassigned after a round of assignment attempts.
 * Some easier puzzles can be solved by calling this function in a loop.
 */
uint8_t Puzzle::apply_logic_rules()
{
    calculate_all_candidates();
    assign_simple_candidates();
    find_and_assign_exclusive_candidates();
    return count_unassigned_cells();
}

/**
 * Assigns symbols to cells that only have one possible candidate.
 */
void Puzzle::assign_simple_candidates()
{
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            uint8_t popcount = std::__popcount(m_candidates[i][j]);
            if (popcount == 1)
            {
                char symbol = symbol::get_first_symbol_from_mask(m_candidates[i][j]);
                m_board[i][j] = symbol;
                m_num_logic_assignments++;
                m_candidates[i][j] = 0;
                remove_symbol_from_candidates_in_constraint_zones(i, j, symbol);
            }
        }
    }
}

/**
 * For every symbol, count the amount of unassigned cells in a constraint 
 * zone (row, column, or square), that could possibly have that symbol.
 * If for some constraint zone there is only one cell that can have a certain
 * symbol, then we can assign it to that cell.
 */
void Puzzle::find_and_assign_exclusive_candidates()
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
                // if the symbol is a candidate for this cell, increase the count of cells found
                if (m_candidates[i][j] & symbol::get_symbol_mask(symbol))
                {
                    cell_count++;
                    candidate_column = j;
                }
            }

            // if we found only one cell that can have this symbol, assign it.
            if (cell_count == 1)
            {
                m_board[i][candidate_column] = symbol;
                m_num_logic_assignments++;
                m_candidates[i][candidate_column] = 0;
                remove_symbol_from_candidates_in_constraint_zones(i, candidate_column, symbol);
            }
        }

        // check cols
        for (int j = 0; j < gridSize; j++)
        {
            int cell_count = 0;
            int candidate_row = 0;
            for (int i = 0; i < gridSize; i++)
            {
                if (m_candidates[i][j] & symbol::get_symbol_mask(symbol))
                {
                    cell_count++;
                    candidate_row = i;
                }
            }

            // if we found only one cell that can have this symbol, assign it.
            if (cell_count == 1)
            {
                m_board[candidate_row][j] = symbol;
                m_num_logic_assignments++;
                m_candidates[candidate_row][j] = 0;
                remove_symbol_from_candidates_in_constraint_zones(candidate_row, j, symbol);
            }
        }

        // check every square
        for (int offset = 0; offset < gridSize; offset++)
        {
            int x = (offset / squareSize) * squareSize;
            int y = (offset % squareSize) * squareSize;

            int cell_count = 0;
            int candidate_row = 0;
            int candidate_col = 0;
            for (int i = x; i < x + squareSize; i++)
            {
                for (int j = y; j < y + squareSize; j++)
                {

                    if (m_candidates[i][j] & symbol::get_symbol_mask(symbol))
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
                m_num_logic_assignments++;
                m_candidates[candidate_row][candidate_col] = 0;
                remove_symbol_from_candidates_in_constraint_zones(candidate_row, candidate_col, symbol);
            }
        }
    }
}
