#include "puzzle.hpp"

const std::string Puzzle::borderColor = Color::green;
const std::string Puzzle::numberColor = Color::teal;

void Puzzle::print_board()
{
    print_dashes(borderColor, 37);
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            std::cout << (j % 3 == 0 ? (borderColor + "| ") : "  ") << Color::end
                      << numberColor << (m_board[i][j] > '0' ? m_board[i][j] : ' ')
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

// You can guarantee that number X goes into cell[i][j] if there is
// no other place in the square that X can go, or no other place
// in the row/column that X can go.
void Puzzle::solve_puzzle()
{
}