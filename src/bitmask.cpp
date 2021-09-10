#include "bitmask.hpp"

uint16_t bitmask::get_symbol_mask(char symbol)
{
    return 1UL << (symbol - '1');
}

// returns the largest symbol from the candidate mask
char bitmask::get_first_symbol_from_mask(uint16_t candidate_mask)
{
    return '0' + (bitmask::uint16_bits - std::__countl_zero(candidate_mask));
}

char bitmask::get_next_symbol_from_mask(uint16_t candidate_mask, char prev_symbol)
{
    if (prev_symbol == '0')
    {
        return get_first_symbol_from_mask(candidate_mask);
    }
    // if prev symbol is set, return the next smallest symbol.
    // zero out all the other symbols
    size_t bit_shift = prev_symbol - '0';
    uint16_t mask = 0xFFFF >> ((bitmask::uint16_bits - bit_shift) + 1);
    return get_first_symbol_from_mask(mask & candidate_mask);
}