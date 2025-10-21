#pragma once

// buffer for the identify device command

#include <cstdint>

#include "drivers/serial/kostream.hpp"

#include "aii/array.hpp"

namespace Drivers::Ahci{
  struct IdentifyDeviceBuffer{
    IdentifyDeviceBuffer();
    void PrintSerialNumber() const;
    void PrintCHS() const;
    void PrintLogicalCHS() const;
    
    std::uint32_t NumSectors() const;
    std::uint16_t CommandSetVector() const{ return buffer[82];}

    std::uint32_t UserAccessibleSectors() const;

    Aii::Array<std::uint16_t, 256> buffer;
  };
}
