
#pragma once
#include <bit>
#include <cstdint>

namespace symbol
{
    const char unassigned_symbol = '0';
    const char first_symbol = '1';
    const char last_symbol = '9';
    const size_t uint16_bits = std::numeric_limits<uint16_t>::digits;
    const size_t uint32_bits = std::numeric_limits<uint32_t>::digits;

    uint8_t get_symbol_index(char symbol);
    uint16_t get_symbol_mask(char symbol);
    char get_first_symbol_from_mask(uint16_t candidate_mask);
    char get_next_symbol_from_mask(uint16_t candidate_mask, char prev_symbol);
}
