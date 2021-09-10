#include "puzzle.hpp"
#include <iomanip>

const std::string Puzzle::borderColor = Color::green;
const std::string Puzzle::symbolColor = Color::teal;

void Puzzle::print_board()
{
    std::cout << std::endl;
    print_dashes(borderColor, 37);
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            std::cout << (j % 3 == 0 ? (borderColor + "| ") : "  ") << Color::end
                      << symbolColor << (m_board[i][j] > '0' ? m_board[i][j] : ' ')
                      << Color::end
                      << " ";
        }

        std::cout << borderColor << "|" << Color::endl;
        if ((i + 1) % 3 == 0)
        {
            print_dashes(borderColor, 37);
        }
        else
        {
            std::cout << std::endl;
        }
    }
}

// returns false if a cell is found to be unassigned and to have no options.
// means that backprop reached a dead end.
// do we need the board[i][j] == 0 check?
bool Puzzle::calculate_candidates(size_t i, size_t j)
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

// You can guarantee that number X goes into cell[i][j] if there is
// no other place in the square that X can go, or no other place
// in the row/column that X can go.
// returns true if the puzzle is solved
uint8_t Puzzle::iter_solve_puzzle()
{

    calculate_all_candidates();
    assign_simple_candidates();
    print_board();
    std::cout << "just executed assign_simple_candidates: " << count_unassigned_cells() << std::endl
              << std::endl
              << std::endl;

    calculate_all_candidates();
    assign_other_candidates1();
    print_board();
    std::cout << "just executed assign_other_candidates: " << count_unassigned_cells() << std::endl
              << std::endl
              << std::endl;

    return count_unassigned_cells();
}