#include "print.hpp"
#include <iostream>
#include "colors.hpp"

void print_dashes(std::string color, int count)
{
    std::cout << color;
    for (int i = 0; i < count; i++)
    {
        std::cout << '-';
    }
    std::cout << Color::endl;
}

void newline()
{
    std::cout << std::endl;
}

void Puzzle::print_candidates(uint8_t i, uint8_t j)
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

void Puzzle::print_all_candidates()
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

/**
 * Prints the board, with colors.
 */
void Puzzle::print_board()
{
    newline();
    print_dashes(Color::borderColor, num_printout_dashes);
    for (uint8_t i = 0; i < gridSize; i++)
    {
        for (uint8_t j = 0; j < gridSize; j++)
        {
            std::cout << (j % squareSize == 0 ? (Color::borderColor + "| ") : "  ") << Color::end
                      << Color::symbolColor << (m_board[i][j] > '0' ? m_board[i][j] : ' ')
                      << Color::end
                      << " ";
        }

        std::cout << Color::borderColor << "|" << Color::endl;
        if ((i + 1) % squareSize == 0)
        {
            print_dashes(Color::borderColor, num_printout_dashes);
        }
        else
        {
            newline();
        }
    }
}

std::string Puzzle::get_puzzle_string()
{
    return std::string((char *)(m_board), sizeof(m_board));
}
