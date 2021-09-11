#include "symbol.hpp"

/**
 * Get the index of the symbol w.r.t it's domain. 
 * i.e. '1' -> 0, '2' -> 1, ... '9' -> 8
 */
uint8_t get_symbol_index(char symbol)
{
    return symbol - '1';
}

/**
 * Returns a 16-bit integer that has only the i_th bit set,
 * with i being the index of the symbol. 
 * Note: Symbols start from 1, but their indeces start from 0.
 * get_symbol_mask('2') -> 0b0000000000000010
 * get_symbol_mask('9') -> 0b0000000100000000
 */
uint16_t symbol::get_symbol_mask(char symbol)
{
    return 1UL << get_symbol_index(symbol);
}

/**
 * Returns the largest symbol from the candidate set.
 */
char symbol::get_first_symbol_from_mask(uint16_t candidate_set)
{
    return zeroSymbol + (symbol::uint16_bits - std::__countl_zero(candidate_set));
}

/**
 * Returns the next largest symbol after @arg{symbol} from the candidate set.
 */
char symbol::get_next_symbol_from_mask(uint16_t candidate_set, char symbol)
{
    if (symbol == zeroSymbol)
    {
        return get_first_symbol_from_mask(candidate_set);
    }
    // mask out bits including/higher than symbol
    size_t num_bits_to_shift = symbol - zeroSymbol;
    uint16_t mask = 0xFFFF >> ((symbol::uint16_bits - num_bits_to_shift) + 1);

    return get_first_symbol_from_mask(mask & candidate_set);
}
