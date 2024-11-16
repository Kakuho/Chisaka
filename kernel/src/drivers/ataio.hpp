#ifndef ATAIO_HPP
#define ATAIO_HPP

#include "drivers/serial/kostream.hpp"

extern "C" void outb(int port, char val);
extern "C" std::uint8_t inb(int port);
extern "C" std::uint8_t dosomething();

namespace Disk{
  constexpr std::uint16_t PrimaryDriveSelectPort = 0x1F6;
  constexpr std::uint16_t CommandPort = 0x1F7;
  constexpr std::uint16_t StatusPort = 0x1F7;
  void InitialiseAtaPio(std::uint8_t drive, std::uint8_t bus);
}

#endif
