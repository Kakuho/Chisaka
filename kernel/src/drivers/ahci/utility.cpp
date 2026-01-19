#include "utility.hpp"
#include "kmalloc.hpp"
#include "aii/string.h"

namespace Drivers::Ahci{

using Pci::ioaddr32_t;

std::uint32_t operator&(std::uint32_t src, CAP_FLAGS flag){
  return src & static_cast<std::uint32_t>(flag);
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

void CheckAhciMode(){
  kassert(IsAhciController());
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;

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

void CheckMemoryAddressAccess(){
  ioaddr32_t subBaseClassOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x04);
  outl(Pci::CONFIG_ADDRESS, subBaseClassOffset);
  const std::uint32_t statusCommand = inl(Pci::CONFIG_DATA);
  const std::uint16_t command = statusCommand & 0xFFFF;
  kout << "AHCI Command: " << command << '\n';
}

//-------------------------------------------------------------
//  Important Address Values
//-------------------------------------------------------------

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

std::uint64_t GetAbar(){
  kassert(IsAhciController());
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  return abar;
}

std::uint64_t GetAhciBase(){
  kassert(IsAhciController());
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;
  return base;
}

std::uint32_t* GetCapPtr(){
  kassert(IsAhciController());
  std::uint64_t base = GetAhciBase();
  std::uint32_t* cap = reinterpret_cast<std::uint32_t*>(base);
  return cap;
}

std::uint32_t* GetGhcPtr(){
  kassert(IsAhciController());
  std::uint64_t base = GetAhciBase();
  std::uint32_t* ghc = reinterpret_cast<std::uint32_t*>(base + 4);
  return ghc;
}

std::uint32_t* GetIsPtr(){
  kassert(IsAhciController());
  std::uint64_t base = GetAhciBase();
  std::uint32_t* is = reinterpret_cast<std::uint32_t*>(base + 8);
  return is;
}

const std::uint32_t* GetPiPtr(){
  kassert(IsAhciController());
  std::uint64_t base = GetAhciBase();
  std::uint32_t* pi = reinterpret_cast<std::uint32_t*>(base + 0xc);
  return pi;
}

//-------------------------------------------------------------
//  Initialisation Routine
//-------------------------------------------------------------

void InitialiseAhci(){
  // This is the System Software Initialisation Portion of the Hba
  kassert(IsAhciController());
  kout << "Initialising Ahci..." << '\n';
  ResetController();
  EnableAhci();
  EnumerateImplementedPorts();
  Aii::Array<bool, 32> implementedPorts = GetImplementedPorts();
  if(!IsHbaIdle()){
    kout << "HBA is not idle, so we force to idle" << '\n';
    ForceHbaIdle();
  }
  kout << "Command Slots: " << GetCommandSlots() << '\n';
  SetupMemory();

  /*
  if(Supports64BitAddress()){
    SetupMemory64FIXED();
  }
  else{
    kassert(false && "Have not implemented Ahci structures setup for 32bit");
  }
  */

  EnablePortsFre();
  ClearAllPortSerr();
  EnableInterrupts();
  kout << "AHCI Initalisation Finished" << '\n';
  kout << "----------------------------------\n";
}

void ResetController(){
  kout << "Resetting the Ahci Controller..." << '\n';
  EnableAhci();
  volatile std::uint32_t* ghc = GetGhcPtr();
  *ghc |= 1;
  while(*ghc & 1){
    ;;
  }
  kout << "Finished Resetting the Ahci Controller" << '\n';
}

bool IsAhciController(){
  ioaddr32_t subBaseClassOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x08);
  outl(Pci::CONFIG_ADDRESS, subBaseClassOffset);
  const std::uint32_t classCode = inl(Pci::CONFIG_DATA) >> 8;
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

bool CheckFirmwareInitialisation(){
  // return false iff firmware has failed in initialising some parts in accordance
  // to the Ahci spec
}

void EnableAhci(){
  kout << "Enabling Ahci Mode (GHC.AE)..." << '\n';
  kassert(IsAhciController());
  volatile std::uint32_t* ghc = GetGhcPtr();
  if(*ghc & 0x8000'0000){
    kout << "Ahci Mode already enabled" << '\n';
  }
  else{
    kout << "Ahci Mode is not enabled" << '\n';
    *ghc |= 0x8000'0000;
    while(!(*ghc & 0x8000'0000)){
      ;
    }
    kout << "Ahci Mode Enabled" << '\n';
  }
  AhciEnabled = true;
  return;
}

void EnumerateImplementedPorts(){
  kassert(AhciEnabled);
  kout << "Enumerating Implemented Ports..." << '\n';
  const std::uint32_t* piRegister = GetPiPtr();
  const std::uint32_t piVal = *piRegister;
  kout << "HBA.PI = " << piVal << '\n';
  for(std::uint8_t i = 0; i < 32; i++){
    if(piVal & (1 << i)){
      kout << "port " << i << " is implemented\n";
    }
  }
}

Aii::Array<bool, 32> GetImplementedPorts(){
  kassert(AhciEnabled);
  Aii::Array<bool, 32> payload{false};
  const std::uint32_t* piRegister = GetPiPtr();
  const std::uint32_t piVal = *piRegister;
  for(std::uint8_t i = 0; i < 32; i++){
    if(piVal & (1 << i)){
      payload[i] = true;
    }
  }
  return payload;
}


bool IsHbaIdle(){
  kassert(AhciEnabled);
  kout << "Checking If HBA is Idle..." << '\n';
  Aii::Array<bool, 32> implementedPorts = GetImplementedPorts();
  for(std::uint8_t i = 0; i < 32; i++){
    if(implementedPorts[i] == true){
      if(!Ports::PortIdle(i)){
        kout << "Port " << i << " is not Idle..." << '\n';
        return false;
      }
    }
  }
  return true;

}

void ForceHbaIdle(){
  kassert(AhciEnabled);
  Aii::Array<bool, 32> implementedPorts = GetImplementedPorts();
  for(std::uint8_t i = 0; i < 32; i++){
    if(implementedPorts[i] == true){
      if(Ports::PortIdle(i)){
        kout << "Port " << i << " is idle" << '\n';
      }
      else{
        kout << "Port " << i << " is not idle, forcing idle..." << '\n';
        Ports::ForceIdle(i);
        kout << "Port " << i << " is now idle" << '\n';
      }
    }
  }
}

std::uint8_t GetCommandSlots(){
  kassert(AhciEnabled);
  std::uint32_t* cap = GetCapPtr();
  return (*cap & NCS) >> 8;
}

bool Supports64BitAddress(){
  kassert(AhciEnabled);
  const std::uint32_t* cap = GetCapPtr();
  return *cap & CAP_FLAGS::S64A ? true: false;
}

void SetupMemory(){
  kassert(AhciEnabled);
  kassert(Supports64BitAddress());
  Aii::Array<bool, 32> implementedPorts = GetImplementedPorts();
  for(std::uint8_t i = 0; i < 32; i++){
    if(implementedPorts[i] == true){
      kout << "Setting up Memory for Port " << i << '\n';
      void* portCommandBase = Kernel::kmalloc(sizeof(CommandList));
      //std::uint64_t portCommandBase = 0x100000 + i*sizeof(CommandList);
      kassert(reinterpret_cast<std::uintptr_t>(portCommandBase) % 0x400 == 0);
      Ports::SetPortClb(i, portCommandBase);
      void* portFisBase = Kernel::kmalloc(sizeof(RecievedFis));
      //std::uint64_t portFisBase = FIS_START + i*sizeof(RecievedFis);
      kassert(reinterpret_cast<std::uintptr_t>(portFisBase) % 0x100 == 0);
      Ports::SetPortFb(i, portFisBase);
      kout << "Finished Setting up Memory for Port " << i << '\n';
    }
  }
}


void SetupMemory64FIXED(){
  // Fixed to a particular address for now,
  // later would be better to assign using heap?
  kassert(AhciEnabled);
  kassert(Supports64BitAddress());
  kout << "Setting up Memory for Command Table and RFis..." << '\n';
  Aii::Memset(reinterpret_cast<void*>(COMMANDLIST_START), 0x00, 32*sizeof(CommandList));
  Aii::Memset(reinterpret_cast<void*>(FIS_START), 0x00, 32*sizeof(RecievedFis));
  kout << "Cleared Memory for Memory Structures" << '\n';
  Aii::Array<bool, 32> implementedPorts = GetImplementedPorts();
  for(std::uint8_t i = 0; i < 32; i++){
    if(implementedPorts[i] == true){
      kout << "Setting up Memory for Port " << i << '\n';
      std::uint64_t portCommandBase = COMMANDLIST_START + i*sizeof(CommandList);
      //std::uint64_t portCommandBase = 0x100000 + i*sizeof(CommandList);
      kassert(portCommandBase % 0x400 == 0);
      Ports::SetPortClb(i, portCommandBase);
      std::uint64_t portFisBase = FIS_START + i*sizeof(RecievedFis);
      kassert(portFisBase % 0x100 == 0);
      Ports::SetPortFb(i, portFisBase);
      kout << "Finished Setting up Memory for Port " << i << '\n';
    }
  }
}

void EnablePortsFre(){
  kassert(AhciEnabled);
  static constexpr std::uint32_t CMD_FRE = 0x10;
  Aii::Array<bool, 32> implementedPorts = GetImplementedPorts();
  for(std::uint8_t i = 0; i < 32; i++){
    if(implementedPorts[i] == true){
      std::uint32_t* portCmd = Ports::GetCmdPtr(i);
      *portCmd |= CMD_FRE;
      kout << "Enabled Port " << i << " FRE" << '\n';
      kout << "Port " << i << " Cmd = " << *Ports::GetCmdPtr(i) << '\n';
    }
  }
}

void ClearAllPortSerr(){
  kassert(AhciEnabled);
  Aii::Array<bool, 32> implementedPorts = GetImplementedPorts();
  for(std::uint8_t i = 0; i < 32; i++){
    if(implementedPorts[i] == true){
      kout << "Port " << i << " Serr is set" << '\n';
      Ports::ClearPortSerr(i);
    }
  }
}

void ClearHostInterruptStatus(){
  kassert(AhciEnabled);
  std::uint32_t* is = GetIsPtr();
  *is |= 0;
  kout << "Hba IS Cleared" << '\n';
}

void ClearPortsInterruptStatus(){
  kassert(AhciEnabled);
  Aii::Array<bool, 32> implementedPorts = GetImplementedPorts();
  for(std::uint8_t i = 0; i < 32; i++){
    if(implementedPorts[i] == true){
      Ports::ClearInterruptStatus(i);
      kout << "Port " << i << " PxIs Cleared" << '\n';
    }
  }
}

void EnableHostInterrupts(){
  kassert(AhciEnabled);
  std::uint32_t* ghc = GetGhcPtr();
  *ghc |= 0b10;
  kout << "Host Interrupts Enabled" << '\n';
}

void EnablePortsInterrupts(){
  kassert(AhciEnabled);
  Aii::Array<bool, 32> implementedPorts = GetImplementedPorts();
  for(std::uint8_t i = 0; i < 32; i++){
    if(implementedPorts[i] == true){
      Ports::EnableInterrupts(i);
      kout << "Port " << i << " Interrupts Enabled" << '\n';
    }
  }
}

void EnableInterrupts(){
  kassert(AhciEnabled);
  ClearPortsInterruptStatus();
  ClearHostInterruptStatus();
  EnablePortsInterrupts();
  EnableHostInterrupts();
}


void StartDMAEngines(){
  kassert(AhciEnabled);
  Aii::Array<bool, 32> implementedPorts = GetImplementedPorts();
  for(std::uint8_t i = 0; i < 32; i++){
    if(implementedPorts[i] == true){
      if(Ports::StartPortDMAEngine(i)){
        kout << "Port " << i << " DMA Engine Started" << '\n';
      }
      else{
        kout << "Failed to start Port " << i << " DMA Engine" << '\n';
      }
    }
  }
}

//-------------------------------------------------------------

void CheckDevicePresent(){
  kassert(AhciEnabled);
  Aii::Array<bool, 32> implementedPorts = GetImplementedPorts();
  for(std::uint8_t i = 0; i < 32; i++){
    if(implementedPorts[i] == true){
      if(Ports::PortHasDevicePresent(i)){
        kout << "Port " << i << " Has a Device Present" << '\n';
      }
      else{
        kout << "Port " << i << " Does Not Have a Device Present" << '\n';
      }
    }
  }
}

//-------------------------------------------------------------

std::uint8_t NumberOfPorts(){
  kassert(Reseted);
  // For now im just gonna get the obvious ones,
  // later i will do complete enumeration
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;
  volatile std::uint32_t* cap = reinterpret_cast<std::uint32_t*>(base);
  return *cap & 0x1F;
}

void EnumerateCapabilities(){
  kassert(Reseted);
  // For now im just gonna get the obvious ones,
  // later i will do complete enumeration
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;
  volatile std::uint32_t* cap = reinterpret_cast<std::uint32_t*>(base);

  kout << "Cap: " << *cap << '\n';

  kout << "Enumerating Capabilities" << '\n';
  // Bit flags

  if(*cap & CAP_FLAGS::S64A){
    kout << "Supports 64-bit Addressing" << '\n';
  }
  if(*cap & CAP_FLAGS::SNCQ){
    kout << "Supports Native Command Queueing" << '\n';
  }
  if(*cap & CAP_FLAGS::SSNTF){
    kout << "Supports SNotification Register" << '\n';
  }
  if(*cap & CAP_FLAGS::SAM){
    kout << "Supports AHCI Mode Only" << '\n';
  }
  // Quantities
  kout << "Number of ports: " << (*cap & 0x1F) << '\n'
       << "Number of Command Slots: " << ((*cap & NCS) >> 8) << '\n';

}

bool SupportsBiosHandoff(){
  // If supports bios handoff, then the BOHC register is supported
  static std::uint32_t BOHMASK = 0x0000'0001;
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;


  // Get CAP2
  volatile std::uint32_t* cap2 = reinterpret_cast<std::uint32_t*>(base + 0x24);
  return *cap2 & BOHMASK;
}

void PrintBiosHandoff(){
  if(SupportsBiosHandoff){
    kout << "Supports BIOS Handoff... has BOHC register" << '\n';
  }
}

void PerformBiosHandoff(){
  kassert(Reseted);
  kout << "Checking if OS owns AHCI HBA..." << '\n';

  // useful bitmasks
  static std::uint32_t BIOS_BUSY            = 0x0000'0010;
  static std::uint32_t OS_OWNERSHIP_CHANGE  = 0x0000'0008;
  static std::uint32_t SOOE                 = 0x0000'0004;
  static std::uint32_t OS_OWNED             = 0x0000'0002;
  static std::uint32_t BIOS_OWNED           = 0x0000'0001;

  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;
  std::uint32_t* bohcRegister = reinterpret_cast<std::uint32_t*>(base + 0x28);
  kout << "initial bohc: " << *bohcRegister << '\n';

  // first check if the os owns hba
  bool osOwned = (*bohcRegister & OS_OWNED) && !(*bohcRegister & BIOS_OWNED);
  if(osOwned){
    kout << "OS owns the AHCI HBA" << '\n';
    return;
  }
  else{
    kout << "OS Does not own the AHCI HBA, Performing Handoff..." << '\n';
    // Enable the SM
    kout << "Setting SOOE" << '\n';
    *bohcRegister |= SOOE;
    kout << "bohc: " << *bohcRegister << '\n';
    // now procedure starts...
    *bohcRegister |= OS_OWNED;
    kout << "bohc: " << *bohcRegister << '\n';
    // After 25msec, check BB
    while(*bohcRegister & BIOS_OWNED){
      continue;
    }
    kout << "BIOS does not own anymore" << '\n';
    // Recheck if the os is owned
    osOwned = *bohcRegister & OS_OWNERSHIP_CHANGE;
    while(!osOwned){
      osOwned = OS_OWNERSHIP_CHANGE;
    }
    kout << "OS now owns the AHCI HBA" << '\n';
    return;
  }
}

}
