#include "bitmask.hpp"

/**
 * Returns a 16-bit integer that has only the i_th bit set,
 * with i being the index of the symbol. 
 * Note: Symbols start from 1, but their indeces start from 0.
 * get_symbol_mask('2') -> 0b0000000000000010
 * get_symbol_mask('9') -> 0b0000000100000000
 */
uint16_t bitmask::get_symbol_mask(char symbol)
{
    return 1UL << (symbol - '1');
}

/**
 * Returns the largest symbol from the candidate set.
 */
char bitmask::get_first_symbol_from_mask(uint16_t candidate_set)
{
    return '0' + (bitmask::uint16_bits - std::__countl_zero(candidate_set));
}

/**
 * Returns the next largest symbol after @arg{symbol} from the candidate set.
 */
char bitmask::get_next_symbol_from_mask(uint16_t candidate_set, char symbol)
{
    if (symbol == '0')
    {
        return get_first_symbol_from_mask(candidate_set);
    }
    // mask out bits including/higher than symbol
    size_t num_bits_to_shift = symbol - '0';
    uint16_t mask = 0xFFFF >> ((bitmask::uint16_bits - num_bits_to_shift) + 1);

    return get_first_symbol_from_mask(mask & candidate_set);
}