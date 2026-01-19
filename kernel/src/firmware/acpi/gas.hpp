#pragma once

// Type to represent the acpi generic address structure

#include <cstdint>

#include "drivers/serial/kostream.hpp"
#include "kassert.hpp"

namespace Chisaka{

  struct [[gnu::packed]] Gas{
    public:
      static const char* AddressSpaceIdString(std::uint8_t id);

    public:
      void PrintValues() const;

    public:
      std::uint8_t addressSpaceId;
      std::uint8_t regBitWidth;
      std::uint8_t regBitOffset;
      std::uint8_t accessSize;
      std::uint64_t address;
  };

  static_assert(sizeof(Gas) == 12);
}
