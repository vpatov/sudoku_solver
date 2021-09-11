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

void Puzzle::remove_symbol_from_candidates_in_constraint_zones(uint8_t row, uint8_t col, char symbol)
{
    uint16_t symbol_mask = bitmask::get_symbol_mask(symbol);
    // row
    for (uint8_t j = 0; j < gridSize; j++)
    {
        if (m_board[row][j] == '0' && (m_candidates[row][j] & symbol_mask))
        {
            m_candidates[row][j] &= ~symbol_mask;
        }
    }

    // col
    for (uint8_t i = 0; i < gridSize; i++)
    {
        if (m_board[i][col] == '0' && (m_candidates[i][col] & symbol_mask))
        {
            m_candidates[i][col] &= ~symbol_mask;
        }
    }

    // 3x3 square
    uint8_t ox = (row / squareSize) * squareSize;
    uint8_t oy = (col / squareSize) * squareSize;
    for (uint8_t i = ox; i < ox + squareSize; i++)
    {
        for (uint8_t j = oy; j < oy + squareSize; j++)
        {
            if (m_board[i][j] == '0' && (m_candidates[i][j] & symbol_mask))
            {
                m_candidates[i][j] &= ~symbol_mask;
            }
        }
    }
}

/**
 * Updates the candidate set at m_candidates[]i[j].
 * Returns early if the cell has no more possible candidates.
 */
void Puzzle::calculate_candidates(uint8_t i, uint8_t j)
{
    if (m_board[i][j] != '0')
    {
        return;
    }

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
        return;
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
        return;
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
        return;
    }
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
    remove_some_candidates();
}

void Puzzle::assign_simple_candidates()
{
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            uint8_t popcount = std::__popcount(m_candidates[i][j]);
            if (popcount == 1)
            {
                char symbol = bitmask::get_first_symbol_from_mask(m_candidates[i][j]);
                m_board[i][j] = symbol;
                m_candidates[i][j] = 0;
            }
        }
    }
}

size_t Puzzle::count_unassigned_cells()
{
    size_t unassigned_cells = 0;
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            if (m_board[i][j] == '0')
            {
                unassigned_cells++;
            }
        }
    }
    return unassigned_cells;
}

/**
 * For every symbol, count the amount of unassigned cells in a constraint 
 * zone (row, column, or 3x3 square), that could possibly have that symbol.
 * If for some constraint zone there is only one cell that can have a certain
 * symbol, then we can assign it to that cell.
 */
void Puzzle::find_and_assign_singular_candidates()
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
                if (m_candidates[i][j] & bitmask::get_symbol_mask(symbol))
                {
                    cell_count++;
                    candidate_column = j;
                }
            }

            // if we found only one cell that can have this symbol, assign it.
            if (cell_count == 1)
            {
                m_board[i][candidate_column] = symbol;
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
                if (m_candidates[i][j] & bitmask::get_symbol_mask(symbol))
                {
                    cell_count++;
                    candidate_row = i;
                }
            }

            // if we found only one cell that can have this symbol, assign it.
            if (cell_count == 1)
            {
                m_board[candidate_row][j] = symbol;
                m_candidates[candidate_row][j] = 0;
                remove_symbol_from_candidates_in_constraint_zones(candidate_row, j, symbol);
            }
        }

        // check every 3x3 square
        for (int offset = 0; offset < gridSize; offset++)
        {
            int x = (offset / squareSize) * squareSize;
            int y = (offset % 3) * squareSize;

            int cell_count = 0;
            int candidate_row = 0;
            int candidate_col = 0;
            for (int i = x; i < x + squareSize; i++)
            {
                for (int j = y; j < y + squareSize; j++)
                {

                    if (m_candidates[i][j] & bitmask::get_symbol_mask(symbol))
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
                m_candidates[candidate_row][candidate_col] = 0;
                remove_symbol_from_candidates_in_constraint_zones(candidate_row, candidate_col, symbol);
            }
        }
    }
}

/**
     * Calculates candidates for all of the cells in the row, column, or 3x3 square
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

// for a 3x3 square, if a symbol is only a candidate for cells in one row/column, then we
// can remove that symbol from any candidate sets for that row/column in other squares.
void Puzzle::remove_some_candidates()
{

    for (int offset = 0; offset < gridSize; offset++)
    {
        int x = (offset / squareSize) * squareSize;
        int y = (offset % 3) * squareSize;

        // you could replace this with a counter, and a row variable, similar to what you did in other methods
        std::unordered_set<size_t> rows_symbol_is_candidate_in[numSymbols];
        std::unordered_set<size_t> cols_symbol_is_candidate_in[numSymbols];

        for (char symbol = '1'; symbol <= '9'; symbol++)
        {
            size_t symbol_index = symbol - '1';
            uint16_t symbol_mask = bitmask::get_symbol_mask(symbol);
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

void Puzzle::backprop()
{
    int row;
    int col;
    int ox;
    int oy;
    int popped_i;
    int popped_j;
    char popped_symbol = '0';
    bool failure;
    int num_cells_initially_unassigned = count_unassigned_cells();
    int total_guesses = 0;
    std::vector<std::tuple<size_t, size_t, char>> stack;

    std::cout
        << Color::purple << "Starting attempt at back-propogation."
        << Color::endl;
    std::cout
        << Color::yellow << "# unassigned cells: " << num_cells_initially_unassigned
        << Color::endl
        << std::endl;

    // find next unassigned cell
find_first_unassigned_cell:;

    for (row = 0; row < gridSize; row++)
    {
        for (col = 0; col < gridSize; col++)
        {
            if (m_board[row][col] == '0')
            {
                goto found_first_unassigned_cell;
            }
        }
    }
    std::cout
        << Color::green << "BACKPROP SUCCESS!!"
        << Color::endl;
    print_board();
    std::cout
        << Color::green << "Required a total of " << total_guesses << " guesses for "
        << num_cells_initially_unassigned << " unassigned cells."
        << Color::endl;
    return;
found_first_unassigned_cell:;

    // if we just failed, then we popped a cell and unassigned it.
    // that cell should always be the first unassigned cell.
    if (failure)
    {
        assert(popped_i == row && popped_j == col);
    }
    char symbol = bitmask::get_next_symbol_from_mask(m_candidates[row][col], popped_symbol);
    // char symbol = get_first_symbol_from_mask(m_candidates[row][col]);
    uint16_t symbol_mask = bitmask::get_symbol_mask(symbol);

    if (m_candidates[row][col] == 0 || symbol == '0')
    {
        std::cout
            << std::endl
            << Color::purple
            << "Going to failure because first unassigned cell at "
            << row << ", " << col << " has no candidates."
            << Color::endl;
        goto failure_label;
    }

    m_board[row][col] = symbol;
    total_guesses++;

    // problem is with this line. when we make an assignment, we permanently remove that candidate
    // from the set for that cell. However, when backprop fails, and we pop up to a previous config,
    // we may well have to try the same candidate again for this cell.
    // i.e.
    // 1, (3|5), (5|7)
    // 1, 5, 7 -> fail
    // 1, 3, !! cant assign 7 because it was removed from candidates
    // removing the candidate was convenient because it made iterating over the candidates easier.
    // this problem can be solved with another matrix that keeps track
    // m_candidates[row][col] &= ~symbol_mask;

    if (row > 9 || col > 9 || symbol == '0')
    {
        std::cout << "breakpoint" << std::endl;
    }
    stack.push_back(std::tuple(row, col, symbol));
    std::cout << std::endl
              << Color::yellow << "(" << stack.size() << ") pushing: " << row << ", " << col << ", " << symbol << Color::endl;
    // std::cout << "1,3 candidates: ";
    // print_candidates(1, 3);
    failure = false;
    popped_symbol = '0';

    // - update backprop candidates with info that we are removing this candidate
    // - remove the just-assigned symbol from candidates of unassigned neighbors
    // - right after removing them, perform the check to make sure there are still candidates

    // row
    for (int j = 0; j < gridSize; j++)
    {
        if (m_board[row][j] == '0' && (m_candidates[row][j] & symbol_mask))
        {
            std::cout << "Removing candidate "
                      << symbol << " from " << row << ", " << j << std::endl;
            std::cout << "Inserting "
                      << (row * gridSize) + j << " into m_backprop at "
                      << row << ", " << col << std::endl;
            m_backprop_candidates[row][col].insert((row * gridSize) + j);

            m_candidates[row][j] &= ~symbol_mask;

            if (m_candidates[row][j] == 0)
            {
                std::cout
                    << Color::teal << "Encountered failure at " << row << ", " << j
                    << Color::endl;

                failure = true;
            }
        }
    }
    // col
    for (int i = 0; i < gridSize; i++)
    {
        if (m_board[i][col] == '0' && (m_candidates[i][col] & symbol_mask))
        {
            std::cout << "Removing candidate " << symbol << " from " << i << ", " << col << std::endl;
            std::cout << "Inserting "
                      << (i * gridSize) + col << " into m_backprop at "
                      << row << ", " << col << std::endl;
            m_backprop_candidates[row][col].insert((i * gridSize) + col);
            m_candidates[i][col] &= ~symbol_mask;

            if (m_candidates[i][col] == 0)
            {
                std::cout << Color::teal << "Encountered failure at " << i << ", " << col << Color::endl;
                failure = true;
            }
        }
    }

    // 3x3 square
    ox = (row / squareSize) * squareSize;
    oy = (col / squareSize) * squareSize;
    for (int i = ox; i < ox + squareSize; i++)
    {
        for (int j = oy; j < oy + squareSize; j++)
        {
            if (m_board[i][j] == '0' && (m_candidates[i][j] & symbol_mask))
            {
                m_backprop_candidates[row][col].insert((i * gridSize) + j);
                std::cout << "Removing candidate " << symbol << " from " << i << ", " << j << std::endl;
                std::cout << "Inserting "
                          << (i * gridSize) + j << " into m_backprop at "
                          << row << ", " << col << std::endl;
                m_candidates[i][j] &= ~symbol_mask;

                if (m_candidates[i][j] == 0)
                {
                    std::cout << Color::teal << "Encountered failure at " << i << ", " << j << Color::endl;
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
                if (m_board[i][j] == '0' && m_candidates[i][j] == 0)
                {
                    failure = true;
                    std::cout << Color::white
                              << "relying on sanity check to set failure at " << i << ", " << j
                              << Color::endl;
                }
            }
        }
    }

    if (failure)
    {
    failure_label:;

        std::cout << Color::red << std::endl
                  << "failure. stack size: " << stack.size() << Color::endl;
        assert(stack.size());
        auto tup = stack.back();

        stack.pop_back();
        popped_i = std::get<0>(tup);
        popped_j = std::get<1>(tup);
        popped_symbol = std::get<2>(tup);
        uint16_t symbol_mask = bitmask::get_symbol_mask(popped_symbol);
        std::cout << "popped: " << popped_i << ", " << popped_j << ", " << popped_symbol << std::endl;

        if (popped_i == 1 && popped_j == 3)
        {
            std::cout << "breakpoint" << std::endl;
        }
        assert(popped_symbol != '0');
        assert(m_board[popped_i][popped_j] == popped_symbol);
        m_board[popped_i][popped_j] = '0';

        for (auto index : m_backprop_candidates[popped_i][popped_j])
        {
            int row = index / gridSize;
            int col = index % gridSize;

            // only restore the candidates if it is not the failed cell.
            // The failed cell needs to keep that candidate removed, so
            // we dont try it again, since it didnt work out.
            if (row != popped_i || col != popped_j)
            {
                std::cout << "Restoring " << popped_symbol << " to candidates of " << row << ", " << col << std::endl;
                m_candidates[row][col] |= symbol_mask;
            }
        }
        m_backprop_candidates[popped_i][popped_j].clear();
    }
    goto find_first_unassigned_cell;
}