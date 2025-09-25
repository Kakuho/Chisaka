#pragma once

#include "table_header.hpp"
#include "gas.hpp"

#include "memory/address.hpp"

#include "aii/array.hpp"

namespace Firmware::Acpi{
  struct [[gnu::packed]] HpetTable{
    constexpr static Aii::Array<char, 4> SIG = {'H', 'P', 'E', 'T'};

    public:
      constexpr bool IsSignatureCorrect() noexcept{return header.signature == SIG;}
      Mem::physaddr64_t BaseAddress(){ return gasAddr.address;}

    public:
      TableHeader header;
      std::uint32_t hardwareid;
      Gas gasAddr;
      std::uint8_t hpetNum;
      std::uint16_t minClockTick;
      std::uint8_t pageProtectionOemAttribute;
  };
  static_assert(sizeof(HpetTable) == 56);
}
