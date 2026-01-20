#include "ahci_port.hpp"

namespace Chisaka::Ahci{

AhciPort::AhciPort()
  :
    m_registers{nullptr},
    m_present{false},
    m_devicePresent{false},
    m_dmaRunning{false}
{
}

bool AhciPort::HasDevice() const{
  // Device Presence Test
  const volatile std::uint32_t& tfd = m_registers->tfd;
  const volatile std::uint32_t& ssts = m_registers->ssts;
  if(tfd & TFD_STS_STATUS_BSY){
    kout << "Status Busy is Set" << '\n';
    return false;
  }
  if(tfd & TFD_STS_STATUS_DRQ){
    kout << "Status Device Request is Set" << '\n';
    return false;
  }
  bool ipmTest =
    ((ssts & SATA_STATUS_IPM >> 8) == 2) ||
    ((ssts & SATA_STATUS_IPM >> 8) == 6) ||
    ((ssts & SATA_STATUS_IPM >> 8) == 8);
  bool sstsTest = ((ssts & SATA_STATUS_DET) == 0x03) || ipmTest;
  if(!sstsTest){
    kout << "SSTS Test Failed" << '\n';
    return false;
  }
  return true;
}

void AhciPort::ClearSERR(){
  // it is by writting 1 we clear it...
  m_registers->serr = 1;
}

void AhciPort::EnableFRE(){
  // set the FIS Recieved Enable bit
  volatile std::uint32_t& cmdstatus = CommandStatus();
  cmdstatus |= CMD_FRE;
  m_fre = true;
}

void AhciPort::DisableFRE(){
  // clear the FIS Recieved Enable bit
  volatile std::uint32_t& cmdstatus = CommandStatus();
  cmdstatus &= ~CMD_FRE;
  m_fre = false;
}

bool AhciPort::Idle() const{
  //  The Following should be cleared if idle:
  //  CMD.ST, CMD.FRE, CMD.FR and CMD.CR
  static std::uint32_t COMBINED_MASK = CMD_ST | CMD_FRE | CMD_FR | CMD_CR;
  if(CommandStatus() & COMBINED_MASK){
    return false;
  }
  return true;
}

void AhciPort::ForceIdle(){
  volatile std::uint32_t cmdstatus = CommandStatus();
  // Checking CMD_ST -> and CMD_CR
  if(cmdstatus & CMD_ST){
    kout << "Clearing CMD.ST" << '\n';
    cmdstatus &= ~CMD_ST;
    while(cmdstatus & CMD_CR){
      ;;
    }
    kout << "CMD.ST Cleared Successfully" << '\n';
  }
  else{
    kout << "CMD.ST already cleared" << '\n';
    if(cmdstatus & CMD_CR){
      kout << "CMD.CR is not cleared..." << '\n';
    }
  }
  //
  if(cmdstatus & CMD_FRE){
    kout << "Clearing CMD.FRE" << '\n';
    cmdstatus &= ~CMD_FRE;
    while(cmdstatus & CMD_FR){
      ;;
    }
    kout << "CMD.ST Cleared Successfully" << '\n';
  }
  else{
    kout << "CMD.ST already cleared" << '\n';
    if(cmdstatus & CMD_FR){
      kout << "CMD.FR is not cleared..." << '\n';
    }
  }
}

void AhciPort::EnableDMAEngine(){
  volatile std::uint32_t& cmd = CommandStatus();
  bool preconditions = !(cmd & CMD_CR) && DevicePresent() && FisRecievedEnabled();
  if(preconditions){
    cmd |= CMD_ST;
    m_dmaRunning = true;
  }
  else{
    kout << "Failed to Start Port DMA Engine, preconditions are false" << '\n';
    m_dmaRunning = false;
  }
}

std::uint64_t AhciPort::CommandListBaseAddr() const{
  volatile std::uint32_t clbLow = CLBLow();
  volatile std::uint32_t clbHigh = CLBHigh();
  std::uint64_t clbBase = 0;
  /*
  if(Supports64BitAddress()){
    clbBase = clbHigh;
    clbBase = (clbBase << 32) | clbLow;
  }
  else{
    clbBase |= clbLow;
  }
  */
  clbBase = clbLow;
  return clbBase;
}

std::uint64_t AhciPort::RecievedFisBaseAddr() const{
  volatile std::uint32_t fisLow = FisLow();
  volatile std::uint32_t fisHigh = FisHigh();
  std::uint64_t fisBase = 0;
  /*
  if(Supports64BitAddress()){
    clbBase = clbHigh;
    clbBase = (clbBase << 32) | clbLow;
  }
  else{
    clbBase |= clbLow;
  }
  */
  fisBase = fisLow;
  return fisBase;
}


int AhciPort::EmptyCommandSlot(){
  // we check the device status (sata active) and the command issue bits to 
  // see if any are empty
  volatile std::uint32_t& sataActive = SataActive();
  volatile std::uint32_t& commandIssue = CommandIssue();
  for(int i = 0; i < 32; i++){
    const std::uint32_t POS_MASK = 1 << i;
    bool sataActiveSet = sataActive & POS_MASK;
    bool commandIssueSet = commandIssue & POS_MASK;
    if(!sataActiveSet && !commandIssueSet){
      return i;
    }
  }
  // couldn't find an empty space in the command list, we return -1
  return -1;
}

void AhciPort::IssueCommand(std::uint8_t slot){
  kassert(slot < 32);
  volatile std::uint32_t& commandIssue = CommandIssue();
  commandIssue |= 1 << slot;
}

bool AhciPort::CommandSlotSet(std::uint8_t slot) const{
  kassert(slot < 32);
  volatile const std::uint32_t& commandIssue = CommandIssue();
  return commandIssue & (1 << slot);
}

void AhciPort::WaitOnDHRS(){
  static constexpr std::uint32_t DHRS_MASK = 1;
  while(IE() & DHRS_MASK){

  }
}

}
