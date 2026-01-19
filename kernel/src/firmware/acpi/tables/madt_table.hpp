#pragma once

//  Reference - 5.2.12
//
//  Multiple APIC Table Definition and supporting structures to implement 
//  MADT

#include "table_header.hpp"

#include "types.hpp"

namespace Firmware::Acpi{

  // Reference ACPI: 5.2.12
  //  - Multiple APIC Definition Table
  struct MadtTable{
    enum class ControllerType: std::uint8_t{
      Lapic, Ioapic, IntOverride, Nmi, LapicNmi, LapicAddrOveride,
      IoSapic, LSapic, 
      PlatformInterrupt, 
      Lx2apic, Lx2apicNmi, 
      Gicc, Gicd, GicMsiFrame, Gicr, Its, reseverved
    };

    struct MadtEntry{
      std::uint8_t type;
      std::uint8_t length;
    };

    public:
      TableHeader header;
      Chisaka::PhysAddr32 lapicAddr;
      std::uint32_t flags;
      MadtEntry* entries;
  };

  // Reference ACPI: table 5-46 LAPIC
  struct Lapic{
    enum class FlagsV{
      Enabled = 0x01,
      OnlineCapable = 0x02
    };
    static constexpr std::uint8_t TYPE = 0;

    std::uint8_t type = 0;
    std::uint8_t length;
    std::uint8_t processorUid;
    std::uint8_t apicId;
    std::uint32_t flags;
  };

  // Reference ACPI: table 5-47 IOAPIC
  struct Ioapic{
    static constexpr std::uint8_t TYPE = 1;

    std::uint8_t type;
    std::uint8_t length;
    std::uint8_t ioapicId;
    std::uint8_t reserved;
    Chisaka::PhysAddr32 ioapicAddr;
    std::uint32_t globalSystemInterruptBase;
  };
}
