#ifndef DRIVERS_PCI_HPP
#define DRIVERS_PCI_HPP

#include <cstdint>

#include "drivers/serial/kostream.hpp"

extern "C" void outl(std::uint32_t address, std::uint32_t data);
extern "C" std::uint32_t inl(std::uint32_t address);

namespace Drivers::Pci{

  using ioaddr32_t = std::uint32_t;
  inline static constexpr ioaddr32_t CONFIG_ADDR = 0xCF8;
  inline static constexpr ioaddr32_t CONFIG_DATA = 0xCFC;

  inline ioaddr32_t FormConfigAddress(
      bool enable, 
      std::uint8_t busn, 
      std::uint8_t devn, 
      std::uint8_t funcn, 
      std::uint8_t regOffset
    )
  {
    std::uint32_t bus  = busn;
    std::uint32_t dev = devn;
    std::uint32_t func = funcn;
    std::uint32_t address = enable;
    address <<= 31;
    address |= bus << 16;
    address |= dev << 11;
    address |= func << 8;
    address |= regOffset & 0xFC;
    return address;
  }

  inline std::uint32_t ReadDeviceHeader(){
    ioaddr32_t address = FormConfigAddress(1, 0, 0, 0, 0);
    outl(CONFIG_ADDR, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    //tmp = (uint16_t)((inl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
    return inl(CONFIG_DATA);
  }

  inline void EnumeratePCI(){
    // brute force method of enumerating available devices on the PCI
    for(std::uint32_t bus = 0; bus < 256; bus++){
      for(std::uint32_t dev = 0; dev < 32; dev++){
        ioaddr32_t address = FormConfigAddress(1, bus, dev, 0, 0);
        outl(CONFIG_ADDR, address);
        std::uint32_t ids = inl(CONFIG_DATA);
        if((ids & 0xFFFF) == 0xFFFF)
          continue;
        kout << intmode::hex;
        kout << "Bus :: " << bus << " :: dev :: " << dev 
             << " :: Vendorid :: " << (ids & 0xFFFF) 
             << " :: Deviceid :: " << ((ids & (0xFFFF << 16)) >> 16) << '\n'
             << "Underlying :: " << ids << '\n'
             << "---" << '\n';
      }
    }
  }
}

#endif
