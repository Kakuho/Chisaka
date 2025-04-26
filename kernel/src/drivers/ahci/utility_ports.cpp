#include "utility_ports.hpp"

namespace Drivers::Ahci::Ports{

using Pci::ioaddr32_t;

//-------------------------------------------------------------
//  Port Registers
//-------------------------------------------------------------

std::uint32_t GetPortBaseAddress(std::uint8_t portno){
  std::uint64_t ahciBase = GetAhciBase();
  return ahciBase + 0x100 + (0x80* portno);
}

std::uint32_t* GetClbPtr(std::uint8_t portno){
  static constexpr std::uint8_t CLB_OFFSET = 0x0;
  std::uint32_t portBase = GetPortBaseAddress(portno);
  return reinterpret_cast<std::uint32_t*>(portBase + CLB_OFFSET);
}

std::uint32_t* GetClbuPtr(std::uint8_t portno){
  static constexpr std::uint8_t CLBU_OFFSET = 0x04;
  std::uint32_t portBase = GetPortBaseAddress(portno);
  return reinterpret_cast<std::uint32_t*>(portBase + CLBU_OFFSET);
}

std::uint32_t* GetFbPtr(std::uint8_t portno){
  static constexpr std::uint8_t FB_OFFSET = 0x08;
  std::uint32_t portBase = GetPortBaseAddress(portno);
  return reinterpret_cast<std::uint32_t*>(portBase + FB_OFFSET);
}

std::uint32_t* GetFbuPtr(std::uint8_t portno){
  static constexpr std::uint8_t FBU_OFFSET = 0x0c;
  std::uint32_t portBase = GetPortBaseAddress(portno);
  return reinterpret_cast<std::uint32_t*>(portBase + FBU_OFFSET);
}

std::uint32_t* GetIsPtr(std::uint8_t portno){
  static constexpr std::uint8_t IS_OFFSET = 0x10;
  std::uint32_t portBase = GetPortBaseAddress(portno);
  return reinterpret_cast<std::uint32_t*>(portBase + IS_OFFSET);
}

std::uint32_t* GetIePtr(std::uint8_t portno){
  static constexpr std::uint8_t IE_OFFSET = 0x14;
  std::uint32_t portBase = GetPortBaseAddress(portno);
  return reinterpret_cast<std::uint32_t*>(portBase + IE_OFFSET);
}

std::uint32_t* GetCmdPtr(std::uint8_t portno){
  static constexpr std::uint8_t CMD_OFFSET = 0x18;
  std::uint32_t portBase = GetPortBaseAddress(portno);
  return reinterpret_cast<std::uint32_t*>(portBase + CMD_OFFSET);
}

std::uint32_t* GetSerrPtr(std::uint8_t portno){
  static constexpr std::uint8_t SERR_OFFSET = 0x30;
  std::uint32_t portBase = GetPortBaseAddress(portno);
  return reinterpret_cast<std::uint32_t*>(portBase + SERR_OFFSET);
}

std::uint32_t* GetCiPtr(std::uint8_t portno){
  static constexpr std::uint8_t CI_OFFSET = 0x38;
  std::uint32_t portBase = GetPortBaseAddress(portno);
  return reinterpret_cast<std::uint32_t*>(portBase + CI_OFFSET);
}

// Read only Pointers

const std::uint32_t* GetTfdPtr(std::uint8_t portno){
  static constexpr std::uint8_t TFD_OFFSET = 0x20;
  std::uint32_t portBase = GetPortBaseAddress(portno);
  return reinterpret_cast<std::uint32_t*>(portBase + TFD_OFFSET);
}

const std::uint32_t* GetSstsPtr(std::uint8_t portno){
  static constexpr std::uint8_t SSTS_OFFSET = 0x28;
  std::uint32_t portBase = GetPortBaseAddress(portno);
  return reinterpret_cast<std::uint32_t*>(portBase + SSTS_OFFSET);
}

//-------------------------------------------------------------
//  Queries
//-------------------------------------------------------------

bool CheckImplementedPort(std::uint8_t port){
  //kassert(Reseted);
  // Check if valid port number
  if(port > (NumberOfPorts() + 1)){
    return false;
  }
  // read from the hba's PI
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;
  volatile std::uint32_t* piRegister = reinterpret_cast<std::uint32_t*>(base + 0xc);
  if(*piRegister & (1 << port)){
    return true;
  }
  // Catch all
  return false;
}

//-------------------------------------------------------------
// Functionalities
//-------------------------------------------------------------

bool PortIdle(std::uint8_t port){
  //  The Following should be cleared if idle:
  //  CMD.ST, CMD.FRE, CMD.FR and CMD.CR
  std::uint32_t* cmdReg = GetCmdPtr(port);
  static std::uint32_t COMBINED_MASK = CMD_ST | CMD_FRE | CMD_FR | CMD_CR;
  if(*cmdReg & COMBINED_MASK){
    return false;
  }
  return true;
}

void ForceIdle(std::uint8_t port){
  kout << "Forcing port " << port << " to idle..." << '\n';
  std::uint32_t* cmdReg = GetCmdPtr(port);
  // Checking CMD_ST -> and CMD_CR
  if(*cmdReg & CMD_ST){
    kout << "Clearing CMD.ST" << '\n';
    *cmdReg &= ~CMD_ST;
    while(*cmdReg & CMD_CR){
      ;;
    }
    kout << "CMD.ST Cleared Successfully" << '\n';
  }
  else{
    kout << "CMD.ST already cleared" << '\n';
    if(*cmdReg & CMD_CR){
      kout << "CMD.CR is not cleared..." << '\n';
    }
  }
  // 
  if(*cmdReg & CMD_FRE){
    kout << "Clearing CMD.FRE" << '\n';
    *cmdReg &= ~CMD_FRE;
    while(*cmdReg & CMD_FR){
      ;;
    }
    kout << "CMD.ST Cleared Successfully" << '\n';
  }
  else{
    kout << "CMD.ST already cleared" << '\n';
    if(*cmdReg & CMD_FR){
      kout << "CMD.FR is not cleared..." << '\n';
    }
  }
}

void SetPortClb(std::uint8_t port, std::uint64_t addr){
  kassert(addr % 0x400 == 0);
  new(reinterpret_cast<CommandList*>(addr)) CommandList{};
  std::uint32_t* clb = GetClbPtr(port);
  std::uint32_t* clbu = GetClbuPtr(port);
  *clb = addr & 0xFFFF'FFFF;
  *clbu = addr >> 32;
}

void SetPortClb(std::uint8_t port, void* addr){ 
  SetPortClb(port, reinterpret_cast<std::uint64_t>(addr));
}

void SetPortFb(std::uint8_t port, std::uint64_t addr){
  kassert(addr % 0x100 == 0);
  new(reinterpret_cast<RecievedFis*>(addr)) RecievedFis{};
  std::uint32_t* fb = GetFbPtr(port);
  std::uint32_t* fbu = GetFbuPtr(port);
  *fb = addr & 0xFFFF'FFFF;
  *fbu = addr >> 32;
}

void SetPortFb(std::uint8_t port, void* addr){ 
  SetPortFb(port, reinterpret_cast<std::uint64_t>(addr));
}

void ClearPortSerr(std::uint8_t port){ 
  *GetSerrPtr(port) = *GetSerrPtr(port);
}

bool PortHasDevicePresent(std::uint8_t port){
  // Useful bitflags
  // PxSTS
  static constexpr std::uint32_t STATUS     = 0xFF;
  static constexpr std::uint32_t STATUS_BSY = 0x80;
  static constexpr std::uint32_t STATUS_DRQ = 0x08;
  // PxSSTS
  static constexpr std::uint32_t SATA_STATUS_IPM  = 0xF00;
  static constexpr std::uint32_t SATA_STATUS_DET  = 0x0F;
  // Device Presence Test
  const std::uint32_t* tfd = GetTfdPtr(port);
  const std::uint32_t* ssts = GetSstsPtr(port);
  if(*tfd & STATUS_BSY){
    kout << "Status Busy is Set" << '\n';
    return false;
  }
  if(*tfd & STATUS_DRQ){
    kout << "Status Device Request is Set" << '\n';
    return false;
  }
  bool ipmTest = 
    ((*ssts & SATA_STATUS_IPM >> 8) == 2) ||
    ((*ssts & SATA_STATUS_IPM >> 8) == 6) ||
    ((*ssts & SATA_STATUS_IPM >> 8) == 8);
  bool sstsTest = ((*ssts & SATA_STATUS_DET) == 0x03) || ipmTest;
  if(!sstsTest){
    kout << "SSTS Test Failed" << '\n';
    return false;
  }
  return true;
}


bool SetFre(std::uint8_t port){
  std::uint32_t* cmd = GetCmdPtr(port);
  if(*cmd & CMD_FRE){
    kout << "FRE already enabled" << '\n';
    return true;
  }
  else{
    kout << "Setting Port " << port << " FRE" << '\n';
    *cmd |= CMD_FRE;
    while(!(*cmd & CMD_FR)){
      ;
    }
    return true;
  }
}

bool ClearFre(std::uint8_t port){
  std::uint32_t* cmd = GetCmdPtr(port);
  // Todo - Implement the prechecks (cmd.st and cmd.cr)
  if(!(*cmd & CMD_FRE)){
    kout << "FRE already disabled" << '\n';
    return true;
  }
  else{
    kout << "Clearing Port " << port << " FRE" << '\n';
    *cmd &= ~CMD_FRE;
    while(*cmd & CMD_FR){
      ;
    }
    return true;
  }
}

bool ClearInterruptStatus(std::uint8_t port){
  std::uint32_t* isPtr = GetIsPtr(port);
  *isPtr = 0;
  return true;
}

bool EnableInterrupts(std::uint8_t port){
  std::uint32_t* iePtr = GetIePtr(port);
  *iePtr = 0x7FC'000'FF;
  return true;
}

bool StartPortDMAEngine(std::uint8_t port){
  std::uint32_t* cmd = GetCmdPtr(port);
  bool preconditions = !(*cmd & CMD_CR) && PortHasDevicePresent(port) && SetFre(port);
  if(preconditions){
    *cmd |= CMD_ST;
    return true;
  }
  else{
    kout << "Failed to Start Port " << port << " DMA Engine" << '\n';
    return false;
  }
}

void SetPortCommandIssue(std::uint8_t port, std::uint8_t slot){
  kassert(slot < 32);
  std::uint32_t* portCi = GetCiPtr(port);
  *portCi |= 1 << slot;
}

bool PortCommandIssueStatus(std::uint8_t port, std::uint8_t slot){
  kassert(slot < 32);
  std::uint32_t* portCi = GetCiPtr(port);
  if(*portCi & (1 << slot)){
    return true;
  }
  else{
    return false;
  }
}

bool CheckPortDHRS(std::uint8_t port){
  std::uint32_t* is = GetIsPtr(port);
  return *is & 0x01;
}

bool CheckPortDPS(std::uint8_t port){
  std::uint32_t* is = GetIsPtr(port);
  return *is & 0x20;
}


bool CheckPortTfdError(std::uint8_t port){
  const std::uint32_t* tfd = GetTfdPtr(port);
  kout << (*tfd & 0xFF00) << '\n';
  return *tfd & 0xFF00;
}

bool CheckPortTfdBsy(std::uint8_t port){
  const std::uint32_t* tfd = GetTfdPtr(port);
  return *tfd & 0x80;
}

bool CheckPortTfdDrq(std::uint8_t port){
  const std::uint32_t* tfd = GetTfdPtr(port);
  return *tfd & 0x08;
}

bool CheckPortTfdErr(std::uint8_t port){
  const std::uint32_t* tfd = GetTfdPtr(port);
  return *tfd & 0x1;
}

//-------------------------------------------------------------
//  Port Structs 
//-------------------------------------------------------------

std::uint64_t GetCommandListBase(std::uint8_t port){
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;

  // find the command list base
  volatile std::uint32_t* portBase = reinterpret_cast<std::uint32_t*>(base + 0x100 + (port * 0x80));
  volatile std::uint32_t* clbLowReg = portBase;
  volatile std::uint32_t* clbHighReg = portBase+1;

  std::uint64_t clbBase = 0;
  if(Supports64BitAddress()){
    clbBase = *clbHighReg;
    clbBase = (clbBase << 32) | *clbLowReg;
  }
  else{
    clbBase |= *clbLowReg;
  }
  return clbBase;

}

void PrintPortCommandListBase(std::uint8_t portno){
  std::uint64_t commandBase = GetCommandListBase(portno);
  kout << intmode::hex;
  kout << "Port " << portno << ", Command List Base: " << commandBase << '\n';
}

int FindEmptyCommandSlot(std::uint8_t port){
  // we assume CMD.FRE and CMD.ST are both set
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;

  // Get the required registers
  volatile std::uint32_t* portBase = reinterpret_cast<std::uint32_t*>(base + 0x100 + (port * 0x80));
  volatile std::uint32_t* portSataActive = portBase + 13;
  volatile std::uint32_t* portCommandIssue = portBase + 14;
 
  for(int i = 0; i < 32; i++){
    bool sataActiveSet = *portSataActive & (1<<i);
    bool commandIssueSet = *portCommandIssue & (1<<i);
    if(!sataActiveSet && !commandIssueSet){
      return i;
    }
  }
  // couldn't find an empty space in the command list, we return -1
  return -1;
}

std::uint64_t GetReceivedBase(std::uint8_t port){
  ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;

  // find the command list base
  volatile std::uint32_t* portBase = reinterpret_cast<std::uint32_t*>(base + 0x100 + (port * 0x80));
  volatile std::uint32_t* fbLowReg = portBase+3;
  volatile std::uint32_t* fbHighReg = portBase+4;

  std::uint64_t fbBase = 0;
  if(Supports64BitAddress()){
    fbBase = *fbHighReg;
    fbBase = (fbBase << 32) | *fbLowReg;
  }
  else{
    fbBase |= *fbLowReg;
  }
  return fbBase;
}

//-------------------------------------------------------------
//  Error Checking
//-------------------------------------------------------------

bool CheckAbortErrors(std::uint8_t portno){
  static constexpr std::uint32_t HBFS = 0x2000'0000;
  std::uint32_t* is = GetIsPtr(portno);
  return *is & HBFS;
}

bool CheckDataErrors(std::uint8_t portno){
  static constexpr std::uint32_t HBDS = 0x1000'0000;
  std::uint32_t* is = GetIsPtr(portno);
  return *is & HBDS;
}

bool CheckSystemMemoryErrors(std::uint8_t portno){
  return CheckAbortErrors(portno) || CheckDataErrors(portno);
}

bool InterfaceErrorsPresent(std::uint8_t portno){
  std::uint32_t* is = GetIsPtr(portno);
  return *is & 0x0c00'0000;
}

//-------------------------------------------------------------
//  Global Structs
//-------------------------------------------------------------

void PrintAllCommandListBase(){
  for(int i = 0; i < 32; i++){
    if(CheckImplementedPort(i)){
      PrintPortCommandListBase(i);
    }
  }
}

}
