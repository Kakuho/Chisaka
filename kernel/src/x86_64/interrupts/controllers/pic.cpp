#include "pic.hpp"

namespace X8664::Interrupt{

PicController::PicController(
    std::uint16_t masterOffset, 
    std::uint16_t slaveOffset
  )
    : m_masterOffset{masterOffset},
      m_slaveOffset{slaveOffset}
{
  Initialise();
}

void PicController::FlushICWs(){
  // Flush the buffered ICWs to the ports
  // ICW1
  outb(MASTER_BASE_PORT, m_masterIcws.icw1);
  outb(SLAVE_BASE_PORT, m_slaveIcws.icw1);
  // ICW2
  outb(MASTER_BASE_PORT+1, m_masterIcws.icw2);
  outb(SLAVE_BASE_PORT+1, m_slaveIcws.icw2);
  // ICW3
  outb(MASTER_BASE_PORT+1, m_masterIcws.icw3);
  outb(SLAVE_BASE_PORT+1, m_slaveIcws.icw3);
  // ICW4
  outb(MASTER_BASE_PORT+1, m_masterIcws.icw4);
  outb(SLAVE_BASE_PORT+1, m_slaveIcws.icw4);
}

void PicController::Initialise(){
  // First set up of the ICWs
  // ICW1
  const std::uint16_t ICW1 = 0x11;
  m_masterIcws.icw1 = ICW1;
  m_slaveIcws.icw1 = ICW1;
  // ICW2
  m_masterIcws.icw2 = m_masterOffset & 0xF8;
  m_slaveIcws.icw2 = m_slaveOffset & 0xF8;
  // ICW3
  m_masterIcws.icw3 = 0x04;
  m_slaveIcws.icw3 = 0x02;
  // ICW4
  m_masterIcws.icw4 = 0x01;
  m_slaveIcws.icw4 = 0x01;
  FlushICWs();
}

//-------------------------------------------------------------
//  Masking
//-------------------------------------------------------------



}
