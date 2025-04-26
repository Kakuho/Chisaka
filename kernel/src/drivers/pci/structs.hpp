#ifndef DRIVERS_PCI_STRUCTS_HPP
#define DRIVERS_PCI_STRUCTS_HPP

// Useful payload structs with PCI values

#include <cstdint>

namespace Drivers::Pci{

struct BarPayload{
  std::uint32_t word0;
  std::uint32_t word1;
  std::uint32_t word2;
  std::uint32_t word3;
  std::uint32_t word4;
  std::uint32_t word5;
};

}

#endif
