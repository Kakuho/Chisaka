#ifndef FIRMWARE_ACPI_DEFINITION_TABLES_HPP
#define FIRMWARE_ACPI_DEFINITION_TABLES_HPP

// Definition of tables which contains definition blocks
//
// A Definition Block contains infomation regarding hardware coded 
// in bytecode for a AML interpreter to parse 

#include <cassert>

#include "tables.hpp"

namespace Firmware::Acpi{
  // Reference: ACPI 5.2.11.1
  //  - Differentiated System Descriptor Table
  struct DsdtTable{
    static constexpr char SIGNATURE[4] = {'D', 'S', 'D', 'T'};
    TableHeader header;
    std::uint8_t* definitionBlock;
  };

  // Reference: ACPI 5.2.11.2
  //  - Secondary System Descriptor Table
  struct SsdtTable{
    static constexpr char SIGNATURE[4] = {'S', 'S', 'D', 'T'};
    TableHeader header;
    std::uint8_t* definitionBlock;
  };
}

#endif
