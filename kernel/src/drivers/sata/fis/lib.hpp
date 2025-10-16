#pragma once

#include <tuple>
#include <cstdint>

// just a general library 

namespace Drivers::Sata::Fis{
  constexpr std::tuple<std::uint16_t, std::uint16_t, std::uint16_t> ExtractAddress(std::uint64_t address){
    // returns in order high, mid, low
    return std::make_tuple(
      address &( 0xFFFFul << 32) >> 32,
      address &( 0xFFFFul << 16) >> 16,
      address & 0xFFFF
    );
  }
}
