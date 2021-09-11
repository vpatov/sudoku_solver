#include "puzzle.hpp"
#include "colors.hpp"
#include "print.hpp"
#include <iomanip>

/*
    Prints the board, with colors.
*/
void Puzzle::print_board()
{
    newline();
    print_dashes(Color::borderColor, 37);
    for (uint8_t i = 0; i < gridSize; i++)
    {
        for (uint8_t j = 0; j < gridSize; j++)
        {
            std::cout << (j % 3 == 0 ? (Color::borderColor + "| ") : "  ") << Color::end
                      << Color::symbolColor << (m_board[i][j] > '0' ? m_board[i][j] : ' ')
                      << Color::end
                      << " ";
        }

        std::cout << Color::borderColor << "|" << Color::endl;
        if ((i + 1) % 3 == 0)
        {
            print_dashes(Color::borderColor, 37);
        }
        else
        {
            newline();
        }
    }
}

void Puzzle::remove_symbol_from_candidates_in_constraint_zones(uint8_t i, uint8_t j, char symbol)
{
}

/**
 * Returns false if a cell is found to be unassigned and to have no candidates.
 */
bool Puzzle::calculate_candidates(uint8_t i, uint8_t j)
{
    if (m_board[i][j] == '0')
    {
        m_candidates[i][j] = 0b111111111;
        // check 3x3 square
        for (int x = ((i / 3) * 3); x < ((i / 3) + 1) * 3; x++)
        {
            for (int y = ((j / 3) * 3); y < ((j / 3) + 1) * 3; y++)
            {
                if (m_board[x][y] != '0')
                {
                    // unset the ith bit for the ith symbol
                    m_candidates[i][j] &= ~(bitmask::get_symbol_mask(m_board[x][y]));
                }
            }
        }
        if (m_candidates[i][j] == 0)
        {
            return false;
        }

        // check row
        for (int y = 0; y < gridSize; y++)
        {
            if (m_board[i][y] != '0')
            {
                m_candidates[i][j] &= ~(bitmask::get_symbol_mask(m_board[i][y]));
            }
        }
        if (m_candidates[i][j] == 0)
        {
            return false;
        }

        // check col
        for (int x = 0; x < gridSize; x++)
        {
            if (m_board[x][j] != '0')
            {
                m_candidates[i][j] &= ~(bitmask::get_symbol_mask(m_board[x][j]));
            }
        }
        if (m_candidates[i][j] == 0)
        {
            return false;
        }
    }
    return true;
}

/*
    Calculate the total amount of possible assignments (legal and illegal) for a given puzzle.
    Equivalent to the product of the number of candidates per cell.
*/
ScientificNotation Puzzle::num_possible_permutations()
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

// You can guarantee that number X goes into cell[i][j] if there is
// no other place in the square that X can go, or no other place
// in the row/column that X can go.
// returns true if the puzzle is solved
uint8_t Puzzle::iter_solve_puzzle()
{
    calculate_all_candidates();
    assign_simple_candidates();
    calculate_all_candidates();
    find_and_assign_singular_candidates();
    return count_unassigned_cells();
}