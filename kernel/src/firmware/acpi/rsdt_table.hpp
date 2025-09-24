#pragma once

//  Reference ACPI: 5.2.7
//  Root System Description Table (RSDT), Contains other acpi description tables. 
//  Must be ignored if XSDT is present in the RSDP table.

#include "table_header.hpp"
#include "memory/address.hpp"
#include "aii/array.hpp"

namespace Firmware::Acpi{

  struct [[gnu::packed]] RsdtTable{
      static constexpr Aii::Array<char, 4> SIG ={
        'R', 'S', 'D', 'T'
      };

    public:
      [[nodiscard]] constexpr bool IsSignatureCorrect() noexcept{
        return header.signature == SIG;
      }

    public:
      TableHeader header;
      Mem::physaddr32_t* tablePointers;
  };
}
