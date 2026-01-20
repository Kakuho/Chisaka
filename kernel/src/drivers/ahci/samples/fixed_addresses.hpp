#pragma once

#include <cstdint>

namespace Drivers::Ahci::Class::Sample{
  static constexpr std::uint64_t DATABUFFER_ADDR     = 0x101000000;
  static constexpr std::uint64_t COMMAND_TABLE_ADDR  = 0x110000000;
  static constexpr std::uint64_t DMAREAD_DATABUFFER_ADDR = 0x101000000;
}
