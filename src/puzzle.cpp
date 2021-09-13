#include "colors.hpp"
#include "puzzle.hpp"
#include "symbol.hpp"
#include "print.hpp"

const std::string Puzzle::puzzle_regex_str = std::string("[0-9]{81}");
const std::regex Puzzle::puzzle_regex = std::regex(puzzle_regex_str);

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
    uint16_t m_candidates_backup[gridSize][gridSize] = {};

    // Back up the candidates such that this function has no side effects.
    memcpy(m_candidates_backup, m_candidates, sizeof(m_candidates));
    calculate_all_candidates();

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

    memcpy(m_candidates, m_candidates_backup, sizeof(m_candidates));
    return sn;
}

/**
 * Solves the puzzle. @returns true if solved, false, if impossible to solve. 
 */
bool Puzzle::solve()
{
    try_to_solve_logically();
    return backtracking();
}

/**
 * Checks whether the puzzle is legal. @returns true if it is, false if it violates the sudoku rules
 * (i.e. multiple occurences of the same symbol in a constraint zone). 
 */
bool Puzzle::is_legal()
{
    std::unordered_set<char> row_symbols[gridSize];
    std::unordered_set<char> col_symbols[gridSize];
    std::unordered_set<char> square_symbols[gridSize];

    for (int i = 0; i < gridSize; i++)
    {

        for (int j = 0; j < gridSize; j++)
        {
            char symbol = m_board[i][j];
            int square_index = ((i / squareSize) * squareSize) + (j / squareSize);

            if (symbol == symbol::unassigned_symbol)
            {
                continue;
            }
            if (row_symbols[i].find(symbol) != row_symbols[i].end())
            {
                return false;
            }
            if (col_symbols[j].find(symbol) != col_symbols[j].end())
            {
                return false;
            }
            if (square_symbols[square_index].find(symbol) != square_symbols[square_index].end())
            {
                return false;
            }
            row_symbols[i].insert(symbol);
            col_symbols[j].insert(symbol);
            square_symbols[square_index].insert(symbol);
        }
    }
    return true;
}

/**
 * Verifies that the puzzle is legal, and tries to solve it if so. 
 * Pretty-prints the solution if one is found, otherwise prints feedback explaining the error.
 */
bool process_puzzle(std::string puzzle_str, int count)
{

    std::cout << "Puzzle " << count << ":" << std::endl;
    if (!std::regex_match(puzzle_str, Puzzle::puzzle_regex))
    {
        std::cout
            << "Puzzle: '" << puzzle_str << "' is invalid." << '\n'
            << "Puzzle strings must follow the pattern: "
            << Puzzle::puzzle_regex_str << '\n'
            << std::endl;
        return false;
    }

    Puzzle puzzle(puzzle_str);
    if (!puzzle.is_legal())
    {
        std::cout
            << Color::red
            << "Puzzle is illegal. Found multiple occurences"
            << " of the same symbol within a constraint zone."
            << '\n'
            << Color::endl;
        return false;
    }

    int num_unassigned_cells = puzzle.count_unassigned_cells();
    ScientificNotation num_possible_permutations = puzzle.num_possible_permutations();

    if (puzzle.solve())
    {
        int num_logic_assignments = puzzle.get_num_logic_assignments();
        int num_backtracking_guesses = puzzle.get_num_backtracking_guesses();

        // std::cout << num_possible_permutations << std::endl;
        std::cout << Color::green << "Out of "
                  << Color::yellow << num_unassigned_cells
                  << Color::green << " initially empty cells, "
                  << Color::yellow << num_logic_assignments
                  << Color::green << " were assigned using logic, and "
                  << Color::yellow << num_unassigned_cells - num_logic_assignments
                  << Color::green << " were assigned using backtracking."
                  << Color::endl;

        if (num_backtracking_guesses > 0)
        {
            std::cout
                << Color::teal << "Backtracking required "
                << Color::yellow << num_backtracking_guesses
                << Color::teal << " guesses (total amount of permutations of candidates for unassigned cells was "
                << Color::purple << num_possible_permutations
                << Color::teal << ")." << Color::endl;
        }

        std::cout << Color::blue << puzzle.get_puzzle_string() << Color::endl;
        puzzle.print_board();
        newline();
        return true;
    }

    std::cout
        << Color::red
        << "Puzzle is impossible to solve. " << std::endl
        << Color::teal << "If you think this is an "
        << "error, please file an issue on github, and provide the puzzle."
        << Color::endl;

    return false;
}