#pragma once

// Interface to an ahci disk connected to an ahci port
//
// The disk is in poll mode, so quite expensive to perform read and writes due 
// to spin locks
//
// The disk represents a sector addressable array

#include <cstdint>

namespace Drivers::Ahci{
  struct AhciDiskPolled{
    public:
      AhciDiskPolled(std::uint8_t port);

      void Read(std::uint16_t sector, std::uint8_t* buffer);
      void Write(std::uint16_t sector, std::uint8_t* buffer);

      void ReadMultiple(std::uint16_t sector, std::uint8_t count, 
          std::uint8_t* buffer);
      void WriteMultiple(std::uint16_t sector, std::uint8_t count, 
          std::uint8_t* buffer);
      
    private:
      std::uint8_t m_port;
      std::size_t m_size;
  };
}
