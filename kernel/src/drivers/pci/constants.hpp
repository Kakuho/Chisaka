#ifndef DRIVERS_PCI_CONSTANTS_HPP
#define DRIVERS_PCI_CONSTANTS_HPP

// Standardised Constants of the PCI Specification

#include <cstdint>

namespace Drivers::Pci{

using ioaddr32_t = std::uint32_t;

inline static constexpr ioaddr32_t CONFIG_ADDRESS = 0xCF8;
inline static constexpr ioaddr32_t CONFIG_DATA = 0xCFC;

}

#endif
