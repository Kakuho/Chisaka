#pragma once

#include <cstdint>

#include "./../utility.hpp"
#include "./../utility_ports.hpp"
#include "./../structs/command_list.hpp"
#include "./../structs/command_header.hpp"
#include "./../structs/command_table.hpp"
#include "./../structs/prdt_entry.hpp"

#include "drivers/sata/commands.hpp"
#include "drivers/sata/fis/h2d_register.hpp"
#include "memory/address.hpp"

namespace Drivers::Ahci::Samples::IdentifyDevice{

// Magic Address - FIXED ADDRESSES FOR NOW

constexpr std::uint64_t DATABUFFER_ADDR     = 0x101000000;
constexpr std::uint64_t COMMAND_TABLE_ADDR  = 0x110000000;

void CheckCommandHeader(CommandHeader& header, void* tablePtr){
  // Forcefully die if the command header is not in the correct format
  kout << "Checking Command Header Fields..." << '\n';
  kout << "Prdtl: " << header.GetPrdtl() << '\n';
  kout << "Port Multiplier: " << header.GetPortMultiplier() << '\n';
  kout << "Clear Busy Upon R_OK: " << header.C() << '\n';
  kout << "BIST: " << header.Bist() << '\n';
  kout << "Reset: " << header.Reset() << '\n';
  kout << "Prefetchable: " << header.Prefetchable() << '\n';
  kout << "Write: " << header.Write() << '\n';
  kout << "Atapi: " << header.Atapi() << '\n';
  kout << "CFL: " << header.Cfl() << '\n';
  kout << "Prdbc: " << header.Prdbc() << '\n';
  kout << "Table Base Address = " << reinterpret_cast<std::uint64_t>(tablePtr) << '\n';
  kout << "Header CMD TBL Base Address = " << header.CommandTableBase() << '\n';
  kassert(header.CommandTableBase() == reinterpret_cast<std::uint64_t>(tablePtr));

  // Testing its internal data members
}

void Try(){
  // Aim: Try to issue identify device to the device
  const std::uint8_t PORT_NUMBER = 0;

  // Required to Perform BIOS OS Handoff??
  
  InitialiseAhci(); 
  StartDMAEngines();

  // Construct the Identify Device Fis
  Sata::Fis::H2DRegister::Frame identifyDeviceFis{};
  identifyDeviceFis.m_commandReg = 0xec;    // 0xec is IDENTIFY DEVICE
  kassert(identifyDeviceFis.m_type == 0x27);
  identifyDeviceFis.m_deviceReg = 0;
  identifyDeviceFis.m_c_portMultiplier = 0x80;

  // Setup Buffer - IDENTIFY DEVICE gives 256 words = 512 bytes
  std::uint16_t* payloadBuffer = reinterpret_cast<std::uint16_t*>(DATABUFFER_ADDR);
  memset(payloadBuffer, 0, 512);
  // 256 words

  // Setup the CommandList's Entry Command Table
  CommandTable* tablePtr = reinterpret_cast<CommandTable*>(COMMAND_TABLE_ADDR);
  tablePtr->SetCommandFis(identifyDeviceFis);
  tablePtr->SetPrdtEntry(0, 
      PrdtEntry{reinterpret_cast<void*>(payloadBuffer), true, 511}
  );

  // Create the Command Header for the Command FIS
  CommandHeader header{
    reinterpret_cast<void*>(tablePtr), 
    1, 
    0b0000'0'100,
    0b100'00101, 
  };
  CheckCommandHeader(header, tablePtr);

  // Find a free command list
  int freeSlot = Ports::FindEmptyCommandSlot(PORT_NUMBER);
  kassert(freeSlot >= 0);
  kout << "Port " << PORT_NUMBER << " has a free Command Header Slot: " << freeSlot << '\n';
  CommandList* portcl = reinterpret_cast<CommandList*>(Ports::GetCommandListBase(PORT_NUMBER));
  portcl->m_headers[freeSlot] = header;
  kout << "Command Header Address: " << reinterpret_cast<std::uint64_t>(&portcl->m_headers[freeSlot]) << '\n';
  kout << "PRDBC : " << portcl->m_headers[freeSlot].m_prdbc << '\n';
  Ports::SetPortCommandIssue(PORT_NUMBER, freeSlot);

  // Now we need to wait for the command to finish
  while(true){
    if(!Ports::PortCommandIssueStatus(PORT_NUMBER, freeSlot)){
      break;
    }
  }
  for(std::uint16_t i = 1; i < 256 + 1; i++){
    kout << payloadBuffer[i-1] << ' ';
    if(i % 32 == 0){
      kout << '\n';
    }
  }
}

}
