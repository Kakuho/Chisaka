#include "segment_descriptor.hpp"
#include "types.hpp"

namespace X8664{

static_assert(sizeof(SegmentDescriptor) == 8);

//---------------------------------------------------------------//
// Lifetime:
//    Functionalities to do with initialisation
//---------------------------------------------------------------//

SegmentDescriptor::SegmentDescriptor(
  void* baseAddress, 
  std::uint16_t limit0,
  std::uint8_t accessByte,
  std::uint8_t flagsLimit1
) noexcept: m_limit0{limit0},
            m_accessbyte{accessByte},
            m_flags_limit1{flagsLimit1}
{
  kout << "Constructing segment descriptor..." << '\n';
  SetBaseAddress(baseAddress);
}

SegmentDescriptor::SegmentDescriptor
  (
    SegmentAttributes::BaseLimit baselimit,
    SegmentAttributes::AccessByte&& accessbyte,
    SegmentAttributes::Flags&& flags
  ) noexcept
{
  SetBaseLimit(baselimit);
  SetAccessByte(accessbyte);
  SetFlags(flags);
}

//---------------------------------------------------------------//
// Helper functions to set the segment attributes:
//---------------------------------------------------------------//

void SegmentDescriptor::SetBaseAddress(void* baseAddress) noexcept{
  // parse the virtual address
  Chisaka::VirtAddr vaddr = 
    reinterpret_cast<Chisaka::VirtAddr>(baseAddress);
  std::uint16_t vaddr0 = vaddr & 0xFFFF;
  std::uint8_t vaddr1 = (vaddr & (0xFFul << 0x10)) >> 0x10;
  std::uint8_t vaddr2 = (vaddr & (0xFFul << 0x18)) >> 0x18;
  // now set the virtual address field 
  m_base0 = vaddr0;
  m_base1 = vaddr1;
  m_base2 = vaddr2;
}

void SegmentDescriptor::SetSegmentLimit(std::uint32_t limit) noexcept{
  kassert(limit <= 0xFFFFF && "limit has to be  <= 0xFFFFF");
  m_limit0 = limit & 0xFFFF;
  // the flag and upper nibble of limit are packed into 
  // one field
  m_flags_limit1 &= 0xF0;   
  m_flags_limit1 |= ((limit & 0xF0000) >> 16);
}

void SegmentDescriptor::SetBaseLimit
  (
    SegmentAttributes::BaseLimit& baseLimit
  ) noexcept
{
  SetBaseAddress(baseLimit.base);
  SetSegmentLimit(baseLimit.limit);
}

void SegmentDescriptor::SetAccessByte
  (
    SegmentAttributes::AccessByte& accessByte
  ) noexcept
{
  // byte buffer for the access byte
  std::uint8_t buffer = accessByte.present;
  buffer <<= 2;
  buffer |= (accessByte.privilaged) ? 0 : 3;
  buffer <<= 1;
  buffer |= (accessByte.isSystemSegment) ? 0 : 1;
  buffer <<= 4;
  buffer |= ((accessByte.type) & 0x0F);
  // now set the member
  m_accessbyte &= 0x0F;
  m_accessbyte = buffer;
}

void SegmentDescriptor::SetAccessByte
  (
    SegmentAttributes::AccessByte&& accessbyte
  ) noexcept
{
  SetAccessByte(accessbyte);
}

void SegmentDescriptor::SetFlags
  (
    SegmentAttributes::Flags& flags
  ) noexcept
{
  // byte buffer for the flag byte
  std::uint8_t flagbyte = !flags.isByteGranular;
  flagbyte <<= 1;
  flagbyte |= flags.db;
  flagbyte <<= 1;
  flagbyte |= flags.longModeSegment;
  flagbyte <<= 1;
  flagbyte |= flags.avl;
  // now we just set the flag byte
  m_flags_limit1 &= 0x0F;
  m_flags_limit1 |= flagbyte << 4;
}

void SegmentDescriptor::SetFlags
  (
    SegmentAttributes::Flags&& flags
  ) noexcept
{
  SetFlags(flags);
}

//---------------------------------------------------------------//
// Attribute Querying
//---------------------------------------------------------------//

[[nodiscard]] std::uint32_t 
SegmentDescriptor::SegmentLimit() const noexcept{
  // returns the segment limit of the segment
  std::uint32_t limit = m_flags_limit1 & 0x0F;
  limit <<= 16;
  limit |= m_limit0;
  return limit;
}

[[nodiscard]] std::uint32_t 
SegmentDescriptor::BaseAddress() const noexcept{
  // returns the base address of the segment
  std::uint32_t base = m_base2 << 8;
  base |= m_base1;
  base <<= 16;
  base |= m_base0;
  return base;
}

// Access Byte

[[nodiscard]] bool
SegmentDescriptor::IsPresent() const noexcept{
  return m_accessbyte & 0x80;
}

[[nodiscard]] bool 
SegmentDescriptor::IsPrivilaged() const noexcept{
  std::uint8_t dpl = (m_accessbyte & 0x70) >> 8;
  return dpl == 0 ? true : false;
}

// Segment Type

[[nodiscard]] bool 
SegmentDescriptor::IsSystemSegment() const noexcept{
  return !(m_accessbyte & 0x10);
}

[[nodiscard]] bool 
SegmentDescriptor::IsDataSegment() const noexcept{
  if(IsSystemSegment()){
    return false;
  }
  else{
    return !(m_accessbyte & 0x08);

  }
}

[[nodiscard]] bool 
SegmentDescriptor::IsCodeSegment() const noexcept{
  if(IsSystemSegment()){
    return false;
  }
  else{
    return m_accessbyte & 0x08;
  }
}

[[nodiscard]] std::uint8_t 
SegmentDescriptor::Type() const noexcept{
  return m_accessbyte & 0xF;
}

// Flags

[[nodiscard]] bool
SegmentDescriptor::IsByteGranular() const noexcept{
  //  output:
  //    true  => segment is measured in bytes
  //    false => segment is measured in 4-KB units
  //  internal: 
  //    set   => 4KB unit
  //    clear => byte unit
  return !(m_flags_limit1 & 0x80);
}

[[nodiscard]] bool
SegmentDescriptor::IsDB() const noexcept{
  return m_flags_limit1 & 0x40;
}

[[nodiscard]] bool
SegmentDescriptor::IsLongCodeSegment() const noexcept{
  // set => the code segment is executed in long mode
  // clear => not long mode
  return m_flags_limit1 & 0x20;
}

}
