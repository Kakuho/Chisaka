#include "ahci_port.hpp"

namespace Drivers::Ahci{

AhciPort::AhciPort()
  :
    m_registers{nullptr},
    m_present{false}
{
}

bool AhciPort::Idle(){
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

}
