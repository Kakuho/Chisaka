#ifndef DRIVERS_AHCI_INIT_HPP
#define DRIVERS_AHCI_INIT_HPP

#include "drivers/pci/constants.hpp"
#include "drivers/pci/pci.hpp"
#include "drivers/serial/kostream.hpp"
#include "memory/address.hpp"

#include "lib/kassert.hpp"

namespace Drivers::Ahci{

static bool Reseted = false;

using Pci::ioaddr32_t;

bool IsAhciController(){
  ioaddr32_t subBaseClassOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x08);
  outl(Pci::CONFIG_ADDRESS, subBaseClassOffset);
  const std::uint32_t classCode = inl(Pci::CONFIG_DATA) >> 8;
  kout << intmode::hex << classCode << '\n';

  const std::uint8_t baseClass = (classCode >> 16) & 0xFF;
  const std::uint8_t subClass = (classCode >> 8) & 0xFF;
  const std::uint8_t programmingInterface = classCode & 0xFF;
  kassert(baseClass == 0x1);
  kassert(subClass == 0x06);
  kassert(programmingInterface == 0x01);
  if((baseClass != 0x1) && (subClass != 0x06) && (programmingInterface != 0x01))
    return false;
  else
    return true;
}

void PrintModeSelect(){
  kassert(IsAhciController());
  ioaddr32_t mapOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x90);
  outl(Pci::CONFIG_ADDRESS, mapOffset);
  const std::uint8_t map = inl(Pci::CONFIG_DATA) & 0xFF;
  switch((map & 0xc0) >> 6){
    case 0:
      kout << "IDE Mode" << '\n';
      break;
    case 1:
      kout << "AHCI Mode" << '\n';
      break;
    case 2:
      kout << "RAID Mode" << '\n';
      break;
    case 3:
      kout << "RESERVED Mode" << '\n';
      break;
    default:
      kout << "Unknown mode" << '\n';
      break;
  }
}

void PrintAbars(){
  kassert(IsAhciController());
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base1 = abar >> 4;
  const std::uint32_t base2 = abar >> 13;
  kout << intmode::hex 
       << "Abar: " << abar << '\n'
       << "Base address: " << base1 << '\n'
       << "Base address: " << base2 << '\n';
}

void CheckAhciMode(){
  kassert(IsAhciController());
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  //const std::uint32_t base = abar >> 13;
  //const std::uint32_t base = Mem::PhysToKVirtAddr(abar >> 13);
  const std::uint32_t base = (abar >> 8) << 8;
  //const std::uint32_t base = Mem::PhysToKVirtAddr(abar >> 4);

  kout << intmode::hex 
       << "Abar: " << abar << '\n'
       << "Base address: " << base << '\n';

  volatile std::uint32_t* ghc = reinterpret_cast<std::uint32_t*>(base + 4);
  kout << "Initialisaing AHCI Mode" << '\n';
  *ghc = 0x8000'0000;
  kout << "ghc: " << *ghc << '\n';

  kout << "Resetting Ahci" << '\n';
  *ghc = 0x8000'0001;
  while(*ghc & 1){
    kout << "General Host: " << *ghc << '\n';
  }
  kout << "ghc: " << *ghc << '\n';
  kout << "Reset Procedure Finished" << '\n';
  Reseted = true;
}

}

#endif
