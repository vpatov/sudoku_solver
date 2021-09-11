
#pragma once
#include <bit>
#include <cstdint>

namespace symbol
{
    const char zeroSymbol = '0';
    const char symbolIndexOffset = '1';
    const size_t uint16_bits = std::numeric_limits<uint16_t>::digits;
    const size_t uint32_bits = std::numeric_limits<uint32_t>::digits;

    uint16_t get_symbol_mask(char symbol);
    char get_first_symbol_from_mask(uint16_t candidate_mask);
    char get_next_symbol_from_mask(uint16_t candidate_mask, char prev_symbol);
}
