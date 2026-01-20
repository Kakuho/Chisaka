#pragma once

#include <cstdint>

#include "fixed_addresses.hpp"
#include "../identify_device_buffer.hpp"

#include "drivers/ahci/class_interface/ahci_driver.hpp"
#include "drivers/ahci/structs/command_list.hpp"
#include "drivers/ahci/structs/command_header.hpp"
#include "drivers/ahci/structs/command_table.hpp"
#include "drivers/ahci/structs/prdt_entry.hpp"

#include "drivers/sata/commands.hpp"
#include "drivers/sata/fis/h2d_register.hpp"

#include "memory/heap/allocator.hpp"
#include "memory/page/page_allocator.hpp"
#include "memory/address.hpp"

#include "aii/string.h"

namespace Drivers::Ahci::Class::Sample{

inline void PrintMaxAddress(){
  // Aim: Try to issue Write to the device

  const std::uint8_t PORT_NUMBER = 0;
  constexpr std::size_t LIMIT = 0x401;

  // Required to Perform BIOS OS Handoff??

  Mem::PageAllocator::Initialise();
  Mem::Heap::Allocator::Initialise();
  auto& ahcidriver = Drivers::Ahci::AhciDriver::Get();
  ahcidriver.Init();

  volatile auto& rfis = ahcidriver.Port(PORT_NUMBER).RecievedFisPtr()->Rfis();

  // setting up the command structures
  auto* cmdTable = Mem::Heap::Allocator::New<Drivers::Ahci::CommandTable>();
  auto readMaxAddrFis = Drivers::Sata::Fis::H2DRegister::Frame::CreateReadMaxAddressFrame();
  cmdTable->SetCommandFis(readMaxAddrFis);

  auto cmdheader = CommandHeader{
    cmdTable,
    0,
    0b0000'0'100, 
    0b000'00101,  
  };

  ahcidriver.Perform(0, cmdheader, cmdTable);
  Drivers::Ahci::AhciDriver::Get().WaitForPortInterrupt(0);

  kout << "Fis Fields:\n"
       << "Error: " << rfis.Error() << '\n'
       << "Low: " << rfis.Low() << '\n'
       << "Mid: " << rfis.Mid() << '\n'
       << "High: " << rfis.High() << '\n';

}

}
