#pragma once

#include <cstdint>

#include "fixed_addresses.hpp"

#include "drivers/ahci/class_interface/ahci_driver.hpp"
#include "drivers/ahci/class_interface/ahci_port.hpp"
#include "drivers/ahci/structs/command_list.hpp"
#include "drivers/ahci/structs/command_header.hpp"
#include "drivers/ahci/structs/command_table.hpp"
#include "drivers/ahci/structs/prdt_entry.hpp"

#include "drivers/sata/commands.hpp"
#include "drivers/sata/fis/h2d_register.hpp"
#include "memory/address.hpp"

#include "aii/string.h"
#include "memory/heap/allocator.hpp"

namespace Drivers::Ahci::Class::Sample{
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

inline std::uint8_t* CreateDataAreaFixed(){
  // Setup Buffer - What do you want to write and how much data?
  std::uint8_t* dataArea = reinterpret_cast<std::uint8_t*>(DATABUFFER_ADDR);
  Aii::Memset(dataArea, 0, 512);
  dataArea[0] = 0b01001101; 
  dataArea[1] = 0b01100101;
  dataArea[2] = 0b01110010;
  dataArea[3] = 0b01101111; 
  dataArea[4] = 0b00100000; 
  dataArea[5] = 0b01001101;
  dataArea[6] = 0b01100101;
  dataArea[7] = 0b01110010; 
  dataArea[8] = 0b01101111;
  dataArea[9] = 0b00100000;
  dataArea[10] = 0b01001101; 
  dataArea[11] = 0b01100101; 
  dataArea[12] = 0b01110010;
  dataArea[13] = 0b01101111;
  dataArea[14] = 0b00100001;
  // there ought to be a much better way to do this no?
  return dataArea;
}

inline std::uint8_t* SetDataArea(std::uint8_t* dataArea){
  // Setup Buffer - What do you want to write and how much data?
  Aii::Memset(dataArea, 0, 512);
  dataArea[0] = 0b01001101; 
  dataArea[1] = 0b01100101;
  dataArea[2] = 0b01110010;
  dataArea[3] = 0b01101111; 
  dataArea[4] = 0b00100000; 
  dataArea[5] = 0b01001101;
  dataArea[6] = 0b01100101;
  dataArea[7] = 0b01110010; 
  dataArea[8] = 0b01101111;
  dataArea[9] = 0b00100000;
  dataArea[10] = 0b01001101; 
  dataArea[11] = 0b01100101; 
  dataArea[12] = 0b01110010;
  dataArea[13] = 0b01101111;
  dataArea[14] = 0b00100001;
  // there ought to be a much better way to do this no?
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
    kout << intmode::bin << data[i-1] << ' ';
    if(i % 8 == 0){
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

inline void WriteRead(){
  // Aim: Try to issue Write to the device

  const std::uint8_t PORT_NUMBER = 0;

  // Required to Perform BIOS OS Handoff??

  Mem::PageAllocator::Initialise();
  Mem::Heap::Allocator::Initialise();
  AhciDriver ahcidriver{};
  ahcidriver.StartDMAEngines();

  auto ahciPorts = ahcidriver.GetPorts();

  auto writeFrame = CreateDmaWriteFrame();
  std::uint8_t* dataArea = CreateDataAreaFixed();
  CommandTable* commandTable = CreateCommandTable(writeFrame, dataArea);
  CommandHeader cmdHead = CreateCommandHeader(commandTable);

  // Find a free command list
  int freeSlot = ahciPorts[PORT_NUMBER].EmptyCommandSlot();
  CommandList* portcl = ahciPorts[PORT_NUMBER].CommandListPtr();
  portcl->m_headers[freeSlot] = cmdHead;
  ahciPorts[PORT_NUMBER].IssueCommand(freeSlot);

  // Now we need to wait for the command to finish
  while(ahciPorts[PORT_NUMBER].CommandSlotSet(freeSlot)){
    // spin lol
    ;;
  }

  kout << "We've written stuff!" << '\n';

  ResetDataArea(dataArea);
  kout << "Prior: " << '\n';
  PrintDataArea(dataArea);


  //-------------------------------------------------------------
  //
  kout << "Attemping to read Sata Disk sector 0 into data area..." << '\n';
  
  auto readFrame = CreateDmaReadFrame();
  commandTable = DmaRead_CreateCommandTable(readFrame, dataArea);
  CommandHeader dmaRead_cmdHead = DmaRead_CreateCommandHeader(commandTable);

  freeSlot = ahciPorts[PORT_NUMBER].EmptyCommandSlot();
  portcl = ahciPorts[PORT_NUMBER].CommandListPtr();
  portcl->m_headers[freeSlot] = dmaRead_cmdHead;
  ahciPorts[PORT_NUMBER].IssueCommand(freeSlot);

  while(ahciPorts[PORT_NUMBER].CommandSlotSet(freeSlot)){
    // spin lol
    ;;
  }

  kout << "Read Finish, Data Area: " << '\n';
  PrintDataArea(dataArea);
}

inline void WriteRead_AhciFuncs(){
  // Aim: Try to issue Write to the device

  const std::uint8_t PORT_NUMBER = 0;

  // Required to Perform BIOS OS Handoff??

  Mem::PageAllocator::Initialise();
  Mem::Heap::Allocator::Initialise();
  AhciDriver ahcidriver{};
  ahcidriver.StartDMAEngines();

  std::uint8_t* dataArea = static_cast<std::uint8_t*>(Mem::Heap::Allocator::Allocate(512));
  SetDataArea(dataArea);

  ahcidriver.WriteSector(PORT_NUMBER, 0, dataArea);

  ResetDataArea(dataArea);
  kout << "Prior: " << '\n';
  PrintDataArea(dataArea);

  kout << "Attemping to read Sata Disk sector 0 into data area..." << '\n';

  ahcidriver.ReadSector(PORT_NUMBER, 0, dataArea);
  PrintDataArea(dataArea);
}

}
