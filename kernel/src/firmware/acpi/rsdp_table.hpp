#pragma once

//  Reference: ACPI 5.2.5 
//  Root System Description Pointer Table RSDP
//  This is the entry point to acpi services, and it contains a physical pointer to the XSDT Table
//  

#include "memory/address.hpp"
#include "table_header.hpp"

namespace Firmware::Acpi{

  struct [[gnu::packed]] RsdpTable{
    public:
      static constexpr Aii::Array<char, 8> SIG ={
        'R', 'S', 'D', ' ', 'P', 'T', 'R', ' '
      };

      [[nodiscard]] constexpr bool IsSignatureCorrect(){
        return signature == SIG;
      }

    public:
      Aii::Array<char, 8> signature;
      std::uint8_t checksum;
      char oemId[6];
      std::uint8_t revision;
      Mem::physaddr32_t rsdtAddr;
      std::uint32_t length;
      Mem::physaddr64_t xsdtAddr;
      std::uint32_t extChecksum_reserved;
  };

  static_assert(sizeof(RsdpTable) == 36);
}
