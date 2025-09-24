#pragma once

#include "table_header.hpp"
#include "aii/array.hpp"

namespace Firmware::Acpi{
  struct HpetTable{
    constexpr static Aii::Array<std::uint8_t, 8> Signature = {1, 2, 3};
    TableHeader header;
  };
}
