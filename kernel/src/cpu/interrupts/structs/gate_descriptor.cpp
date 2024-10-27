#include "gate_descriptor.hpp"

namespace X8664{

//---------------------------------------------------------------//
//  Lifetime 
//---------------------------------------------------------------//

GateDescriptor::GateDescriptor(){
  // the only invariant we can give is that the type field is a 
  // Idt Call gate
  SetSegmentType();
  kassert(Type() == CALL_GATE_TYPE);
}

GateDescriptor::GateDescriptor(
    std::uint64_t offset, 
    std::uint16_t segmentSelector, 
    bool present
) noexcept:
    m_zeros_ist{0},
    m_reserved{0}
{
  kout << "initialising a Gate GateDescriptor" << '\n';
  this->m_segmentSelector = segmentSelector;
  this->m_accessByte = 0b000'0000| CALL_GATE_TYPE;
  SetOffset(offset);
  SetSegmentType();
  if(present){
    SetPresent();
  }
  else{
    ClearPresent();
  }
  /*
  // series of test to ensure correct logic
  kassert(Offset() == offset);
  kassert(SegmentSelector() == segmentSelector);
  kassert(Type() == CALL_GATE_TYPE);
  kout << m_accessByte << '\n';
  kassert(IsPivilaged() == true);
  kassert(IsPresent() == present);
  */
}

GateDescriptor::GateDescriptor
(
  InterruptAttributes::GateAttributes args
) noexcept: m_zeros_ist{0},
            m_reserved{0}
{
  SetOffset(args.offset);
  SetSegmentSelector(args.segmentSelector);
  SetSegmentType();
  SetPrivilageLevel();
  if(args.isPresent){
    SetPresent();
  }
  else{
    ClearPresent();
  }
}

//---------------------------------------------------------------//
//  Observers
//---------------------------------------------------------------//

[[nodiscard]] std::uint64_t GateDescriptor::Offset() const noexcept 
{
  std::uint64_t addr = m_offset2;
  addr <<= 16;
  addr |= m_offset1;
  addr <<= 16;
  addr |= m_offset0;
  return addr;
}

[[nodiscard]] std::uint16_t 
GateDescriptor::SegmentSelector() const noexcept 
{
  return m_segmentSelector;
}

[[nodiscard]] std::uint8_t 
GateDescriptor::Ist() const noexcept {
  return m_zeros_ist & 0x0F;
}

[[nodiscard]] 
std::uint8_t GateDescriptor::Type() const noexcept
{
  return m_accessByte & 0xF;
}

[[nodiscard]] 
bool GateDescriptor::IsPivilaged() const noexcept
{
  return m_accessByte & (0b0110 << 4) ? false : true;
}

[[nodiscard]] 
bool GateDescriptor::IsPresent() const noexcept
{
  return m_accessByte & 0x80;
}

//---------------------------------------------------------------//
//  Mutators
//---------------------------------------------------------------//

void GateDescriptor::SetOffset(std::uint64_t offset) 
{
  std::uint16_t vaddr0 = offset & 0xFFFF;
  std::uint16_t vaddr1 = (offset & (0xFFFFull << 0x10)) >> 0x10 ;
  std::uint32_t vaddr2 = (offset & (0xFFFF'FFFFul << 0x20)) >> 0x20;
  // setting the internal data
  this->m_offset0 = vaddr0;
  this->m_offset1 = vaddr1;
  this->m_offset2 = vaddr2;
}

void GateDescriptor::SetOffset(void* offset)
{
  SetOffset(
      reinterpret_cast<std::uint64_t>(offset)
  );
}

void 
GateDescriptor::SetSegmentSelector(std::uint16_t segmentSelector){
  m_segmentSelector = segmentSelector;
}

void GateDescriptor::SetSegmentType()
{
  // preserve the top nibble 
  m_accessByte = m_accessByte & 0xF0;
  m_accessByte |= CALL_GATE_TYPE;
}

void GateDescriptor::SetPrivilageLevel(){
  m_accessByte = m_accessByte & 0x8F;
}

void GateDescriptor::SetPresent()
{
  // top bit is the present flag
  m_accessByte |= 0x80;
}

void GateDescriptor::ClearPresent()
{
  m_accessByte &= 0x7F;
}

} // namespace X8664
