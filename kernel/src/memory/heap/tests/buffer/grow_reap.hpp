#pragma once

#include <cstdint>

#include "./../../allocator.hpp"
#include "./../../list_descriptor.hpp"

// TO BE TESTED AFTER THE ALLOCATOR CLASS IS IMPLEMENTED

namespace Mem::Heap::Test::Buffer::GrowReap{

  static constexpr std::uintptr_t BASE_ADDR1 = 0x55000;
  static constexpr std::uintptr_t BASE_ADDR2 = 0x56000;
  static constexpr std::uintptr_t BASE_ADDR3 = 0x57000;
  static constexpr std::uintptr_t BASE_ADDR4 = 0x58000;
  static constexpr std::uintptr_t BASE_ADDR5 = 0x59000;
  static constexpr std::uintptr_t BASE_ADDR6 = 0x60000;
  static constexpr std::uintptr_t BASE_ADDR7 = 0x61000;
  static constexpr std::uintptr_t BASE_ADDR8 = 0x62000;

  static constexpr std::uintptr_t VBASE_ADDR = 0xFFFF800000055000;
  static constexpr std::uint16_t PAGE_SIZE = 0x1000;

}
