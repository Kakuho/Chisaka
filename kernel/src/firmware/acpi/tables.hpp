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

namespace Firmware::Acpi{

  // Reference: ACPI: 5.2.6
  //  - Most ACPI table shares this common header
  struct TableHeader{
    char signature[4];
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
    char signature[8];
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
  class [[gnu::packed]] RsdtTable{
      [[nodiscard]] const Mem::physaddr32_t* 
      operator[](std::size_t index) const;

    private:
      TableHeader header;
      Mem::physaddr32_t* tablePointers;
  };

  // Reference ACPI: 5.2.8
  //  - Extended System Description Table (XSDT)
  class [[gnu::packed]] XsdtTable{
      [[nodiscard]] const Mem::physaddr64_t* 
      operator[](std::size_t index) const;

    private:
      TableHeader header;
      Mem::physaddr64_t* tablePointers;
  };
}

#endif
