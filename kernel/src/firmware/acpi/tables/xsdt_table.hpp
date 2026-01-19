#pragma once

//  Reference ACPI: 5.2.8
//  Extended System Description Table (XSDT)
//  Must be used if the RSDP table contains a XSDT pointer

#include "table_header.hpp"

#include "aii/array.hpp"
#include "types.hpp"

namespace Chisaka{
  class [[gnu::packed]] XsdtTable{
    static constexpr Aii::Array<char, 4> SIG ={'X', 'S', 'D', 'T'};

    public:
      constexpr bool IsSignatureCorrect(){ return header.signature == SIG;}
      [[nodiscard]] const Chisaka::PhysAddr64* operator[](std::size_t index) const;

    private:
      TableHeader header;
      Chisaka::PhysAddr64* tablePointers;
  };
}
