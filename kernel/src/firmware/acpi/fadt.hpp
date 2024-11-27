#ifndef FIRMWARE_ACPI_FADT_HPP
#define FIRMWARE_ACPI_FADT_HPP

// Reference - 5.2.9
//
// The Fixed ACPI Description Table definition is separated from the 
// other table definitions as it is quite a large structure

#include "tables.hpp"

namespace Firmware::Acpi{
  struct FadtTable{
    TableHeader header;
    Mem::physaddr32_t firmwareCtrl; // ptr to FACS
    Mem::physaddr32_t dsdt;         // ptr to DSDT
    std::uint8_t reserved = 0;
    std::uint8_t preferredProfile;
    std::uint16_t sciInt;
    std::uint32_t smiCmdPort;
    std::uint8_t acpiEnable;
    std::uint8_t acpiDisable;
    // THERE's MORE FIELDS TO ADD!!!
  };
}

#endif
