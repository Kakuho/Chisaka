#ifndef FIRMWARE_ACPI_TABLES_HPP
#define FIRMWARE_ACPI_TABLES_HPP

//  ACPI Table Definitions
//
//  Contains:
//  * RSDP
//  * RSDT
//  * XSDT
//  * MADT
// 
//  These structures are given the packed attribute if required

#include <cstdint>
#include <cassert>

#include "memory/address.hpp"
#include "primrose/static_array.hpp"
#include "drivers/serial/kostream.hpp"

namespace Firmware::Acpi{

  // Reference: ACPI: 5.2.6
  //  - Most ACPI table shares this common header
  struct TableHeader{
    Prim::StaticArray<char, 4> signature;
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

  // Reference: ACPI 5.2.5 
  //  - Root System Description Pointer RSDP
  struct [[gnu::packed]] RsdpTable{
    static constexpr Prim::StaticArray<char, 8> SIG ={
      'R', 'S', 'D', ' ', 'P', 'T', 'R', ' '
    };

    public:
      [[nodiscard]] constexpr bool IsSignatureCorrect(){
        return signature == SIG;
      }

      Prim::StaticArray<char, 8> signature;
      std::uint8_t checksum;
      char oemId[6];
      std::uint8_t revision;
      Mem::physaddr32_t rsdtAddr;
      std::uint32_t length;
      Mem::physaddr64_t xsdtAddr;
      std::uint32_t extChecksum_reserved;
  };

  static_assert(sizeof(RsdpTable) == 36);

  // Reference ACPI: 5.2.7
  //  - Root System Description Table (RSDT)
  struct [[gnu::packed]] RsdtTable{
      static constexpr Prim::StaticArray<char, 4> SIG ={
        'R', 'S', 'D', 'T'
      };

    public:
      [[nodiscard]] constexpr bool IsSignatureCorrect() noexcept{
        return header.signature == SIG;
      }

      /*
      [[nodiscard]] const Mem::physaddr32_t* 
      operator[](std::size_t index) const noexcept{
        // indexes the table pointers
        if(index > header.length){
          return nullptr;
        }
        else{
          return tablePointers[index];
        }
      }
      */

    public:
      TableHeader header;
      Mem::physaddr32_t* tablePointers;
  };

  // Reference ACPI: 5.2.8
  //  - Extended System Description Table (XSDT)
  class [[gnu::packed]] XsdtTable{
      static constexpr Prim::StaticArray<char, 4> SIG ={
        'X', 'S', 'D', 'T'
      };

    public:
      [[nodiscard]] const Mem::physaddr64_t* 
      operator[](std::size_t index) const;

    private:
      TableHeader header;
      Mem::physaddr64_t* tablePointers;
  };
}

#endif
