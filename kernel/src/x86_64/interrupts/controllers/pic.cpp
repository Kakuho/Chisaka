#include "pic.hpp"

namespace X8664::Interrupt{

PicController::PicController(std::uint16_t vectorOffset)
  : m_masterOffset{vectorOffset}, 
    m_slaveOffset{static_cast<std::uint8_t>(vectorOffset+8)}
{
  // linear offsets
  Initialise();
  ClearMasks();
}

PicController::PicController(
    std::uint16_t masterOffset, 
    std::uint16_t slaveOffset
  )
    : m_masterOffset{masterOffset},
      m_slaveOffset{slaveOffset}
{
  // non linear offsets
  Initialise();
  ClearMasks();
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
  // 0x20 & 0xF8 = 0x20
  // ICW3
  m_masterIcws.icw3 = 0x04;
  m_slaveIcws.icw3 = 0x02;
  // ICW4
  m_masterIcws.icw4 = 0x01;
  m_slaveIcws.icw4 = 0x01;
  FlushICWs();
}

void PicController::FlushICWs(){
  // Flush the buffered ICWs to the ports
  // Master pic
  outb(MASTER_BASE_PORT, m_masterIcws.icw1);
  outb(MASTER_BASE_PORT+1, m_masterIcws.icw2);
  outb(MASTER_BASE_PORT+1, m_masterIcws.icw3);
  outb(MASTER_BASE_PORT+1, m_masterIcws.icw4);
  // slave pic
  outb(SLAVE_BASE_PORT, m_slaveIcws.icw1);
  outb(SLAVE_BASE_PORT+1, m_slaveIcws.icw2);
  outb(SLAVE_BASE_PORT+1, m_slaveIcws.icw3);
  outb(SLAVE_BASE_PORT+1, m_slaveIcws.icw4);
}

void PicController::Enable(){
  ClearMasks();
}

void PicController::Disable(){
  MaskAll();
}

void PicController::SignalEOI(std::uint8_t irq){
  if(irq < 8){
	  outb(MASTER_BASE_PORT, 0x20);
  }
  else{
		outb(SLAVE_BASE_PORT, 0x20);
	  outb(MASTER_BASE_PORT, 0x20);
  }
}

[[nodiscard]] constexpr std::uint8_t 
PicController::IrqToVector(std::uint8_t irq) const noexcept{
  kassert(irq < 16);
  if(irq < 8){
    return m_masterOffset + irq;
  }
  else{
    return m_slaveOffset + (irq - 8);
  }
}

[[nodiscard]] std::uint16_t 
PicController::Masksbm() const noexcept{
  // returns 16 bit packed bit mask
  std::uint8_t masterbm = inb(MASTER_BASE_PORT+1);
  std::uint8_t slavebm = inb(SLAVE_BASE_PORT+1);
  return (masterbm << 8) | slavebm;
}

void PicController::ClearMasks() noexcept{
    outb(MASTER_BASE_PORT+1, 0);
    outb(SLAVE_BASE_PORT+1, 0);
}
    
void PicController::MaskIrq(std::uint8_t irq) noexcept{
  kassert(irq < 15);
  if(irq < 8){
    std::uint8_t masks = inb(MASTER_BASE_PORT+1);
    masks |= 1 << irq;
    outb(MASTER_BASE_PORT+1, masks);
  } 
  else{
    std::uint8_t masks = inb(SLAVE_BASE_PORT+1);
    masks |= 1 << (irq - 8);
    outb(SLAVE_BASE_PORT+1, masks);
  }
}

void PicController::MaskAll(){
  // essentially, masks all the IRQs
  outb(MASTER_BASE_PORT+1, 0xFF);
  outb(SLAVE_BASE_PORT+1, 0xFF);
}

void PicController::SetIrq(std::uint8_t irq) noexcept{
  kassert(irq < 15);
  if(irq < 8){
    std::uint8_t masks = inb(MASTER_BASE_PORT+1);
    masks &= ~(1 << irq);
    outb(MASTER_BASE_PORT+1, masks);
  } 
  else{
    std::uint8_t masks = inb(SLAVE_BASE_PORT+1);
    masks &= ~(1 << (irq - 8));
    outb(SLAVE_BASE_PORT+1, masks);
  }
}

[[nodiscard]] std::uint16_t 
PicController::GetIrr() noexcept{
  static constexpr std::uint8_t OCW3_IRR = 0x0a;
  outb(MASTER_BASE_PORT, OCW3_IRR);
  outb(SLAVE_BASE_PORT, OCW3_IRR);
  return(inb(SLAVE_BASE_PORT+1) << 8) | inb(MASTER_BASE_PORT+1);
}

[[nodiscard]] std::uint16_t 
PicController::GetIsr() noexcept{
  static constexpr std::uint8_t OCW3_ISR = 0x0b;
  outb(MASTER_BASE_PORT, OCW3_ISR);
  outb(SLAVE_BASE_PORT, OCW3_ISR);
  return(inb(SLAVE_BASE_PORT+1) << 8) | inb(MASTER_BASE_PORT+1);
}

}
