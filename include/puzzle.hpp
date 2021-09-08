#pragma once
#include <string>
#include <iostream>
#include "colors.hpp"
#include <unordered_map>
#include <unordered_set>

class Puzzle
{
    const static int gridSize = 9;

    char m_board[gridSize][gridSize];
    const static std::string borderColor;
    const static std::string numberColor;

    // maps numbers to the cells that they could possibly be in
    std::unordered_map<char, std::unordered_set<size_t>> candidate_map;

    // numbers_in_rows[0] = contains the set of numbers present in the 1st row
    std::unordered_set<char> numbers_in_rows[gridSize];
    std::unordered_set<char> numbers_in_cols[gridSize];

public:
    Puzzle(char board[9][9])
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

    void initialize_helper_vars()
    {
        // std::unordered_set<char> numbers_in_rows[gridSize];
        // std::unordered_set<char> numbers_in_cols[gridSize];
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
                if (m_board[i][j] != '0')
                {
                    numbers_in_rows[i].insert(m_board[i][j]);
                    numbers_in_cols[j].insert(m_board[i][j]);
                }
            }
        }
    }

    void initialize_candidate_map()
    {
        for (int i = 0; i < gridSize; i++)
        {
            for (int j = 0; j < gridSize; j++)
            {
            }
        }
    }

    void print_board();
    void solve_puzzle();
};
