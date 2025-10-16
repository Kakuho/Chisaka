#include "interrupt_manager.hpp"

namespace X8664{
//-------------------------------------------------------------
//  Lifetime
//-------------------------------------------------------------

InterruptManager::InterruptManager(){
  InitialiseGates();
  m_idtr.SwitchLoad(m_table);
}

void InterruptManager::InitialiseGates(){
  // procedure to initialise the call gates of the interrupts
  for(std::uint8_t i = 0; i < 240; i++){
    if(i >= 32){
      InstallISR(i, Null);
    }
    else{
      InstallISR(i, reinterpret_cast<void(*)()>(isr[i]));
    }
    /*
    InstallISR(i, reinterpret_cast<void(*)()>(
          reinterpret_cast<std::uint64_t>(isr[i])&0xFFFFFFFF
    ));
    */
  }
}

//-------------------------------------------------------------
//  Interrupt Service Requests
//-------------------------------------------------------------

void InterruptManager::InstallISR(std::uint16_t index, void* handler){
  m_table[index] = GateDescriptor
  {
    InterruptAttributes::GateAttributes
    {
      .offset = reinterpret_cast<std::uint64_t>(handler),
      .segmentSelector = KERNEL_CODE_SEGMENT,
      .isPresent = true,
    }
  };
}

void InterruptManager::InstallISR(std::uint16_t index, void(*handler)()){
  m_table[index] = GateDescriptor
  {
    InterruptAttributes::GateAttributes
    {
      .offset = reinterpret_cast<std::uint64_t>(handler),
      .segmentSelector = KERNEL_CODE_SEGMENT,
      .isPresent = true,
    }
  };
}

}
