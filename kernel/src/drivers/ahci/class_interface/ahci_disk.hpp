#pragma once

// represents a single disk attached to an ahci port
// is a higher level of abstraction to interact with the ahci driver

#include <cstdint>

namespace Drivers::Ahci{
  struct AhciDisk{
    static constexpr std::size_t SECTOR_BYTES = 512;
    public:
      AhciDisk() = default;
      AhciDisk(std::uint8_t port);

      void SetTotalSectors(std::uint32_t sectors){ m_totalSectors = sectors;}
      std::uint32_t TotalSectors() const{ return m_totalSectors;}
      std::uint32_t TotalBytes() const{ return m_totalSectors * SECTOR_BYTES;}
      std::size_t SectorSize() const{ return SECTOR_BYTES;}

      // Sector IO

      void Read(std::uint16_t sector, std::uint8_t* buffer);
      void ReadPolled(std::uint16_t sector, std::uint8_t* buffer);

      void Write(std::uint16_t sector, std::uint8_t* buffer);
      void WritePolled(std::uint16_t sector, std::uint8_t* buffer);

    private:
      std::uint8_t m_port;
      std::uint32_t m_totalSectors;
  };
}
