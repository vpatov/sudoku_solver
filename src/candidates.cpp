#include "puzzle.hpp"
#include "symbol.hpp"

/**
 * Iterates over the board and initializes + calculates candidates for each cell,
 * based on what symbols a row, column, or square are missing.
 */
void Puzzle::calculate_all_candidates()
{
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {

            // if the cell is empty, calculate possible candidates by going through
            // the square, rows, and cols.
            calculate_candidates(i, j);
        }
    }
    narrow_down_candidates();
}

/**
 * Calculates candidates for all of the cells in the row, column, or square
 * of cell (i,j).
 */
void Puzzle::calculate_candidates_for_constraint_zone(int x, int y)
{
    // check row
    for (int j = 0; j < gridSize; j++)
    {
        calculate_candidates(x, j);
    }

    // check col
    for (int i = 0; i < gridSize; i++)
    {
        calculate_candidates(i, y);
    }

    // check square
    int ox = (x / squareSize) * squareSize;
    int oy = (y / squareSize) * squareSize;

    for (int i = ox; i < x + squareSize; i++)
    {
        for (int j = oy; j < oy + squareSize; j++)
        {
            calculate_candidates(i, j);
        }
    }
}

/**
 * Updates the candidate set at m_candidates[]i[j].
 * Returns early if the cell has no more possible candidates.
 */
void Puzzle::calculate_candidates(uint8_t i, uint8_t j)
{
    if (m_board[i][j] != symbol::unassigned_symbol)
    {
        return;
    }

    m_candidates[i][j] = 0b111111111;

    // check square
    for (int x = ((i / squareSize) * squareSize); x < ((i / squareSize) + 1) * squareSize; x++)
    {
        for (int y = ((j / squareSize) * squareSize); y < ((j / squareSize) + 1) * squareSize; y++)
        {
            if (m_board[x][y] != symbol::unassigned_symbol)
            {
                // unset the ith bit for the ith symbol
                m_candidates[i][j] &= ~(symbol::get_symbol_mask(m_board[x][y]));
            }
        }
    }
    if (m_candidates[i][j] == 0)
    {
        return;
    }

    // check row
    for (int y = 0; y < gridSize; y++)
    {
        if (m_board[i][y] != symbol::unassigned_symbol)
        {
            m_candidates[i][j] &= ~(symbol::get_symbol_mask(m_board[i][y]));
        }
    }
    if (m_candidates[i][j] == 0)
    {
        return;
    }

    // check col
    for (int x = 0; x < gridSize; x++)
    {
        if (m_board[x][j] != symbol::unassigned_symbol)
        {
            m_candidates[i][j] &= ~(symbol::get_symbol_mask(m_board[x][j]));
        }
    }
    if (m_candidates[i][j] == 0)
    {
        return;
    }
}

/**
 * For a square, if a symbol is only a candidate for cells in one row/column, then we
 * can remove that symbol from any candidate sets for that row/column in other squares.
 */
void Puzzle::narrow_down_candidates()
{

    for (int offset = 0; offset < gridSize; offset++)
    {
        int x = (offset / squareSize) * squareSize;
        int y = (offset % squareSize) * squareSize;

        // you could replace this with a counter, and a row variable, similar to what you did in other methods
        std::unordered_set<size_t> rows_symbol_is_candidate_in[numSymbols];
        std::unordered_set<size_t> cols_symbol_is_candidate_in[numSymbols];

        for (char symbol = symbol::first_symbol; symbol <= symbol::last_symbol; symbol++)
        {
            uint8_t symbol_index = symbol::get_symbol_index(symbol);
            uint16_t symbol_mask = symbol::get_symbol_mask(symbol);
            for (int i = x; i < x + squareSize; i++)
            {
                for (int j = y; j < y + squareSize; j++)
                {
                    if (m_candidates[i][j] & symbol_mask)
                    {
                        rows_symbol_is_candidate_in[symbol_index].insert(i);
                        cols_symbol_is_candidate_in[symbol_index].insert(j);
                    }
                }
            }

            // if size is 1, we can remove this symbol from cells in this row in other squares
            if (rows_symbol_is_candidate_in[symbol_index].size() == 1)
            {
                size_t row = *rows_symbol_is_candidate_in[symbol_index].begin();
                for (int j = 0; j < y; j++)
                {
                    m_candidates[row][j] &= ~symbol_mask;
                }
                for (int j = y + squareSize; j < gridSize; j++)
                {
                    m_candidates[row][j] &= ~symbol_mask;
                }
            }
            if (cols_symbol_is_candidate_in[symbol_index].size() == 1)
            {
                size_t col = *cols_symbol_is_candidate_in[symbol_index].begin();
                for (int i = 0; i < x; i++)
                {
                    m_candidates[i][col] &= ~symbol_mask;
                }
                for (int i = x + squareSize; i < gridSize; i++)
                {
                    m_candidates[i][col] &= ~symbol_mask;
                }
            }
        }
    }
}

/**
 * Assumes that @arg{symbol} was just assigned to the cell at @arg{row}, @arg{col}, and removes 
 * the symbol from the constraint zones that that cell is in. calculate_all_candidates would have
 * the same effect, but this function is more efficient because it does much less work for the same
 * result.
 */
void Puzzle::remove_symbol_from_candidates_in_constraint_zones(uint8_t row, uint8_t col, char symbol)
{
    uint16_t symbol_mask = symbol::get_symbol_mask(symbol);
    // row
    for (uint8_t j = 0; j < gridSize; j++)
    {
        if (m_board[row][j] == symbol::unassigned_symbol && (m_candidates[row][j] & symbol_mask))
        {
            m_candidates[row][j] &= ~symbol_mask;
        }
    }

    // col
    for (uint8_t i = 0; i < gridSize; i++)
    {
        if (m_board[i][col] == symbol::unassigned_symbol && (m_candidates[i][col] & symbol_mask))
        {
            m_candidates[i][col] &= ~symbol_mask;
        }
    }

    // square
    uint8_t ox = (row / squareSize) * squareSize;
    uint8_t oy = (col / squareSize) * squareSize;
    for (uint8_t i = ox; i < ox + squareSize; i++)
    {
        for (uint8_t j = oy; j < oy + squareSize; j++)
        {
            if (m_board[i][j] == symbol::unassigned_symbol && (m_candidates[i][j] & symbol_mask))
            {
                m_candidates[i][j] &= ~symbol_mask;
            }
        }
    }
}
