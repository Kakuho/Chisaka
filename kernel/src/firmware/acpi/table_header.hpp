#pragma once

// Reference: ACPI: 5.2.6
//  - ACPI table shares this common header

#include <cstdint>
#include "aii/array.hpp"

namespace Firmware::Acpi{

  struct TableHeader{
    Aii::Array<char, 4> signature;
    std::uint32_t length;
    std::uint8_t revision;
    std::uint8_t checksum;
    char oemId[6];
    char oemTableId[8];
    std::uint32_t oemRevision;
    std::uint32_t creatorId;
    std::uint32_t creatorRevision;
  };

  static_assert(sizeof(TableHeader) == 36);
}
