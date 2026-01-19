#pragma once

// Reference - 5.2.9
//
// The Fixed ACPI Description Table definition is separated from the 
// other table definitions as it is quite a large structure

#include "table_header.hpp"
#include "aii/array.hpp"

#include "types.hpp"

namespace Chisaka{
  struct FadtTable{
    static constexpr Aii::Array<char, 4> SIG = {'F', 'A', 'C', 'P'};
    public:
      constexpr bool IsSignatureCorrect() noexcept{return header.signature == SIG;}

    public:
      TableHeader header;
      Chisaka::PhysAddr32 firmwareCtrl; // ptr to FACS
      Chisaka::PhysAddr32 dsdt;         // ptr to DSDT
      std::uint8_t reserved = 0;
      std::uint8_t preferredProfile;
      std::uint16_t sciInt;
      std::uint32_t smiCmdPort;
      std::uint8_t acpiEnable;
      std::uint8_t acpiDisable;
      // THERE's MORE FIELDS TO ADD!!!
  };
}
