#pragma once

#include <cstdint>

namespace Drivers::Ahci{
  struct AhciBuffer{
    public:
      AhciBuffer(std::uint8_t sectors);

    public:
      std::uint8_t m_sectors;
      char* m_buffer;
  };
}
