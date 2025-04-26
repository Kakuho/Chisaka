#ifndef DRIVERS_PCI_FUNCTIONS_HPP
#define DRIVERS_PCI_FUNCTIONS_HPP

// Functions to get PCI Device Functions values

#include <cstdint>

#include "pci.hpp"
#include "structs.hpp"
#include "constants.hpp"

namespace Drivers::Pci{

inline std::uint32_t GetIdentification(
    std::uint8_t busn, 
    std::uint8_t devn, 
    std::uint8_t funcn
  )
{
  ioaddr32_t address = FormConfigAddress(1, busn, devn, funcn, 0);
  outl(CONFIG_ADDR, address);
  std::uint32_t ids = inl(CONFIG_DATA);
  return ids;
}

inline std::uint32_t GetStatusCommand(
    std::uint8_t busn, 
    std::uint8_t devn, 
    std::uint8_t funcn
  )
{
  ioaddr32_t address = FormConfigAddress(1, busn, devn, funcn, 0x04);
  outl(CONFIG_ADDR, address);
  std::uint32_t statusCommand = inl(CONFIG_DATA);
  return statusCommand;
}

inline std::uint32_t GetClassCode_RevisionId(
    std::uint8_t busn, 
    std::uint8_t devn, 
    std::uint8_t funcn
  )
{
  ioaddr32_t address = FormConfigAddress(1, busn, devn, funcn, 0x04);
  outl(CONFIG_ADDR, address);
  std::uint32_t statusCommand = inl(CONFIG_DATA);
  return statusCommand;
}

BarPayload GetBaseAddressRegisters(
    std::uint8_t busn, 
    std::uint8_t devn, 
    std::uint8_t funcn
  )
{

}

inline std::uint8_t GetCapabilityPointer(
    std::uint8_t busn, 
    std::uint8_t devn, 
    std::uint8_t funcn
  )
{
  ioaddr32_t address = FormConfigAddress(1, busn, devn, funcn, 0x34);
  outl(CONFIG_ADDR, address);
  std::uint32_t word = inl(CONFIG_DATA);
  return word & 0xFF;
}

inline std::uint8_t GetInterruptPin(
    std::uint8_t busn, 
    std::uint8_t devn, 
    std::uint8_t funcn
  )
{
  ioaddr32_t address = FormConfigAddress(1, busn, devn, funcn, 0x3c);
  outl(CONFIG_ADDR, address);
  std::uint32_t word = inl(CONFIG_DATA);
  return word & 0xFF;
}

inline std::uint8_t GetInterruptLine(
    std::uint8_t busn, 
    std::uint8_t devn, 
    std::uint8_t funcn
  )
{
  ioaddr32_t address = FormConfigAddress(1, busn, devn, funcn, 0x3c);
  outl(CONFIG_ADDR, address);
  std::uint32_t word = inl(CONFIG_DATA);
  return (word >> 8) & 0xFF;
}

}

#endif
