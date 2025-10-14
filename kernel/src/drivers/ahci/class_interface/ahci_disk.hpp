#pragma once

#include <cstdint>

// represents a single disk attached to an ahci port
// is a higher level of abstraction to interact with the ahci driver

namespace Drivers::Ahci{
  struct AhciDisk{
    public:
      AhciDisk() = default;
      AhciDisk(std::uint8_t port);

      void SetTotalSectors(std::uint32_t sectors){ m_totalSectors = sectors;}

      void ReadSector(std::uint16_t sector, std::uint8_t* buffer);
      void ReadSectorPolled(std::uint16_t sector, std::uint8_t* buffer);

      void WriteSector(std::uint16_t sector, std::uint8_t* buffer);
      void WriteSectorPolled(std::uint16_t sector, std::uint8_t* buffer);

    private:
      std::uint8_t m_port;
      std::uint32_t m_totalSectors;
  };
}
