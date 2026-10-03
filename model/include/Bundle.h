#pragma once

#include <array>
#include <cstdint>
#include <bitset>

struct Bundle {
    std::bitset<32> active_mask;
    std::array<uint32_t, 32> addr;
};