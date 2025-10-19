#pragma once

#include <tuple>
#include <cstdint>

// just a general library 

namespace Drivers::Sata::Fis{
  constexpr std::tuple<std::uint16_t, std::uint16_t, std::uint16_t, std::uint16_t, std::uint16_t, std::uint16_t> ExtractAddress(std::uint64_t address){
    // returns in order high, mid, low
    return std::make_tuple(
        address & 0xFF,           // lba [7 : 0]
        (address >> 8) & 0xFF,    // lba [15: 8]
        (address >> 16) & 0xFF,   // lba [23: 16]
        (address >> 24) & 0xFF,   // lba [31: 24]
        (address >> 32) & 0xFF,   // lba [39: 32]
        (address >> 40) & 0xFF   // lba [47: 40]
    );
  }
}
