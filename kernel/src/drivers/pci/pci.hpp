#ifndef DRIVERS_PCI_HPP
#define DRIVERS_PCI_HPP

#include <cstdint>

#include "identification.hpp"
#include "constants.hpp"
#include "drivers/serial/kostream.hpp"

extern "C" void outl(std::uint32_t address, std::uint32_t data);
extern "C" void outw(std::uint32_t address, std::uint32_t data);
extern "C" std::uint32_t inl(std::uint32_t address);

namespace Drivers::Pci{

  constexpr ioaddr32_t FormConfigAddress(
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

  inline void EnumeratePCI(){
    // brute force method of enumerating available devices on the PCI
    for(std::uint32_t bus = 0; bus < 256; bus++){
      for(std::uint32_t dev = 0; dev < 32; dev++){
        for(std::uint8_t func = 0; func < 10; func++){
          ioaddr32_t address = FormConfigAddress(1, bus, dev, func, 0);
          outl(CONFIG_ADDRESS, address);
          std::uint32_t ids = inl(CONFIG_DATA);
          if((ids & 0xFFFF) == 0xFFFF)
            continue;
          kout << intmode::hex;
          kout << "Bus :: " << bus << " :: dev :: " << dev 
               << " :: Vendorid :: " << (ids & 0xFFFF) 
               << " :: Deviceid :: " << ((ids & (0xFFFF << 16)) >> 16) << '\n'
               << "Underlying :: " << ids << '\n';
          ioaddr32_t classAddr = FormConfigAddress(1, bus, dev, func, 0x08);
          outl(CONFIG_ADDRESS, classAddr);
          std::uint32_t classes = inl(CONFIG_DATA);
          if(classes == 0xFFFF)
            continue;
          kout << intmode::hex
               << classAddr << '\n'
               << classes << '\n'
               << "Subclass :: " << ((classes >> 16) & 0xFF) << '\n'
               << "Main Class :: " << ((classes >> 24) & 0xFF ) << '\n';
          kout << "----\n";
        }
      }
    }
  }

  inline void PrintEnumeratePCI(){
    // Prettier PCI Enumeration
    // brute force method of enumerating available devices on the PCI
    for(std::uint32_t bus = 0; bus < 256; bus++){
      for(std::uint32_t dev = 0; dev < 32; dev++){
        for(std::uint8_t func = 0; func < 10; func++){
          //-------------------------------------------------------------
          ioaddr32_t address = FormConfigAddress(1, bus, dev, func, 0);
          outl(CONFIG_ADDRESS, address);
          std::uint32_t ids = inl(CONFIG_DATA);
          // Check to see if Vendor is valid
          if((ids & 0xFFFF) == 0xFFFF){
            continue;
          }
          kout << intmode::hex;
          kout << "[" << bus << ":" <<  dev << ":" << func << "] - Probe Success\n";
          PrintIdentifier(ids);
          kout << '\n';
          //-------------------------------------------------------------
          ioaddr32_t classAddr = FormConfigAddress(1, bus, dev, func, 0x08);
          outl(CONFIG_ADDRESS, classAddr);
          std::uint32_t classes = inl(CONFIG_DATA);
          PrintClass(classes);
          kout << '\n';
          //-------------------------------------------------------------
          ioaddr32_t interruptAddr = FormConfigAddress(1, bus, dev, func, 0x3c);
          outl(CONFIG_ADDRESS, interruptAddr);
          std::uint32_t interruptInfo = inl(CONFIG_DATA);
          PrintInterruptInfo(interruptInfo);
          kout << "\n\n";
        }
      }
    }
  }

  inline void CheckSataICH9(){
    ioaddr32_t address = FormConfigAddress(0, 0, 31, 2, 0);
    outl(CONFIG_ADDRESS, address);
    std::uint32_t ids = inl(CONFIG_DATA);
    kout << ids << '\n';
    if((ids & 0xFFFF) == 0xFFFF)
      return;
    kout << intmode::hex;
    kout << "Bus :: " << 1 << " :: dev :: " << 31
         << " :: Vendorid :: " << (ids & 0xFFFF) 
         << " :: Deviceid :: " << ((ids & (0xFFFF << 16)) >> 16) << '\n'
         << "Underlying :: " << ids << '\n'
         << "---" << '\n';

    // checking what type it is

    ioaddr32_t classAddr = FormConfigAddress(0, 0, 31, 2, 0x0a);
    outl(CONFIG_ADDRESS, classAddr);
    std::uint16_t classes = inl(CONFIG_DATA);
    if((classes & 0xFFFF) == 0xFFFF)
      return;
    kout << intmode::hex
         << classAddr << '\n'
         << classes << '\n'
         << "Subclass :: " << (classes & 0xFF) << '\n'
         << "Main Class :: " << ((classes >> 8) & 0xFF )
         << '\n';

  }
}

#endif
