#pragma once

#include "table_header.hpp"
#include "aii/array.hpp"

namespace Firmware::Acpi{
  struct HpetTable{
    constexpr static Aii::Array<char, 4> SIG = {'H', 'P', 'E', 'T'};
    public:
      constexpr bool IsSignatureCorrect() noexcept{return header.signature == SIG;}

    public:
      TableHeader header;
  };
}
