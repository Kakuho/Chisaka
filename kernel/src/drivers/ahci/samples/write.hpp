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

#include "aii/string.h"

namespace Drivers::Ahci::Samples::Write{
  
// Magic Addresses
static constexpr std::uint64_t DATABUFFER_ADDR = 0x101000000;
static constexpr std::uint64_t COMMAND_TABLE_ADDR = 0x110000000;

static constexpr std::uint64_t DMAREAD_DATABUFFER_ADDR = 0x101000000;

inline Sata::Fis::H2DRegister::Frame CreateDmaWriteFrame(){
  // Construct the DMA WRITE EXT H2D Command Fis
  static constexpr std::uint8_t DMA_WRITE_EXT_CODE = 0x35;

  Sata::Fis::H2DRegister::Frame dmaWriteFis{};
  kassert(dmaWriteFis.m_type == 0x27);
  dmaWriteFis.m_sectorCountReg = 1;

  // LBA
  dmaWriteFis.m_lbaLowReg0 = 0;
  dmaWriteFis.m_lbaLowReg1 = 0;
  dmaWriteFis.m_lbaMidReg0 = 0;
  dmaWriteFis.m_lbaMidReg1 = 0;
  dmaWriteFis.m_lbaHighReg0 = 0;
  dmaWriteFis.m_lbaHighReg1 = 0;

  // Rest of the features
  dmaWriteFis.m_deviceReg = 0x40;
  dmaWriteFis.m_c_portMultiplier = 0x80;
  dmaWriteFis.m_commandReg = DMA_WRITE_EXT_CODE;
  return dmaWriteFis;
}

inline std::uint8_t* CreateDataArea(){
  // Setup Buffer - What do you want to write and how much data?
  std::uint8_t* dataArea = reinterpret_cast<std::uint8_t*>(DATABUFFER_ADDR);
  Aii::Memset(dataArea, 0, 512);
  dataArea[0] = 0xDE;
  dataArea[1] = 0xAD;
  dataArea[2] = 0xBE;
  dataArea[3] = 0xEF;
  return dataArea;
}

inline CommandTable* CreateCommandTable(Sata::Fis::H2DRegister::Frame& cfis, void* dataArea){
  CommandTable* tablePtr = reinterpret_cast<CommandTable*>(COMMAND_TABLE_ADDR);
  tablePtr->SetCommandFis(cfis);
  tablePtr->SetPrdtEntry(0, 
      PrdtEntry{reinterpret_cast<void*>(dataArea), true, 511}
  );
  return tablePtr;
}

inline CommandHeader CreateCommandHeader(void* cmdtblPtr){
  return CommandHeader{
    cmdtblPtr, 
    1, 
    0b0000'0'100,
    0b010'00101, 
  };
}

//-------------------------------------------------------------

inline void ResetDataArea(std::uint8_t* data){
  for(std::uint32_t i = 0; i < 512; i++){
    data[i] = 0xAA;
  }
}

inline void PrintDataArea(std::uint8_t* data){
  for(std::uint32_t i = 1; i < 512 + 1; i++){
    kout << data[i-1] << ' ';
    if(i % 32 == 0){
      kout << '\n';
    }
  }
}

inline Sata::Fis::H2DRegister::Frame CreateDmaReadFrame(){
  // Construct the DMA READ EXT H2D Command Fis
  static constexpr std::uint8_t DMA_WRITE_EXT_CODE = 0x25;

  Sata::Fis::H2DRegister::Frame dmaReadFis{};
  kassert(dmaReadFis.m_type == 0x27);
  dmaReadFis.m_sectorCountReg = 1;

  // LBA
  dmaReadFis.m_lbaLowReg0 = 0;
  dmaReadFis.m_lbaLowReg1 = 0;
  dmaReadFis.m_lbaMidReg0 = 0;
  dmaReadFis.m_lbaMidReg1 = 0;
  dmaReadFis.m_lbaHighReg0 = 0;
  dmaReadFis.m_lbaHighReg1 = 0;

  // Rest of the features
  dmaReadFis.m_deviceReg = 0x40;
  dmaReadFis.m_c_portMultiplier = 0x80;
  dmaReadFis.m_commandReg = DMA_WRITE_EXT_CODE;
  return dmaReadFis;
}

inline CommandTable* DmaRead_CreateCommandTable(Sata::Fis::H2DRegister::Frame& cfis, void* dataArea){
  CommandTable* tablePtr = reinterpret_cast<CommandTable*>(COMMAND_TABLE_ADDR);
  tablePtr->SetCommandFis(cfis);
  tablePtr->SetPrdtEntry(0, 
      PrdtEntry{reinterpret_cast<void*>(dataArea), true, 511}
  );
  return tablePtr;
}

inline CommandHeader DmaRead_CreateCommandHeader(void* cmdtblPtr){
  return CommandHeader{
    cmdtblPtr, 
    1, 
    0b0000'0'100,
    0b000'00101, 
  };
}

//-------------------------------------------------------------

inline void Try(){
  // Aim: Try to issue Write to the device

  const std::uint8_t PORT_NUMBER = 0;

  kassert(1 == 2);

  // Required to Perform BIOS OS Handoff??
  
  InitialiseAhci(); 
  StartDMAEngines();

  auto writeFrame = CreateDmaWriteFrame();
  std::uint8_t* dataArea = CreateDataArea();
  CommandTable* commandTable = CreateCommandTable(writeFrame, dataArea);
  CommandHeader cmdHead = CreateCommandHeader(commandTable);

  // Find a free command list
  int freeSlot = Ports::FindEmptyCommandSlot(PORT_NUMBER);
  CommandList* portcl = reinterpret_cast<CommandList*>(Ports::GetCommandListBase(PORT_NUMBER));
  portcl->m_headers[freeSlot] = cmdHead;
  Ports::SetPortCommandIssue(PORT_NUMBER, freeSlot);

  // Now we need to wait for the command to finish
  while(Ports::PortCommandIssueStatus(PORT_NUMBER, freeSlot)){
    ;
  }

  kout << "We've written stuff!" << '\n';

  ResetDataArea(dataArea);
  kout << "Prior: " << '\n';
  PrintDataArea(dataArea);

  //-------------------------------------------------------------
  
  auto readFrame = CreateDmaReadFrame();
  commandTable = DmaRead_CreateCommandTable(readFrame, dataArea);
  CommandHeader dmaRead_cmdHead = DmaRead_CreateCommandHeader(commandTable);

  freeSlot = Ports::FindEmptyCommandSlot(PORT_NUMBER);
  portcl = reinterpret_cast<CommandList*>(Ports::GetCommandListBase(PORT_NUMBER));
  portcl->m_headers[freeSlot] = dmaRead_cmdHead;
  Ports::SetPortCommandIssue(PORT_NUMBER, freeSlot);

  while(Ports::PortCommandIssueStatus(PORT_NUMBER, freeSlot)){
    ;
  }

  kout << "After: " << '\n';
  PrintDataArea(dataArea);
}

}
