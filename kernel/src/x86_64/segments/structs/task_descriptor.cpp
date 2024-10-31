#include "task_descriptor.hpp"

namespace X8664{

static_assert(sizeof(TaskDescriptor) == 16);
  
//---------------------------------------------------------------//
// Lifetime
//---------------------------------------------------------------//

TaskDescriptor::TaskDescriptor(
  void* baseAddress, 
  std::uint32_t limit
) noexcept: 
  // initialise the segment descriptor
  // I probably should simplify this...
  m_segdesc{
    baseAddress, 
    static_cast<std::uint16_t>(limit & 0xFF),
    0b1000'1001,  // present = 1, plvl = 0, system = 1, type = 001, 
    static_cast<std::uint8_t>((limit & (0xF << 0x10)) >> 0x10), 
        // top byte of flagslimit is zero
  }
{
  kout << "Initialising TSS" << '\n';
  // checks to ensure correctness of the Task Descriptor:
  kassert(SegmentLimit() == limit);
  kout << "BaseAddress() = " << BaseAddress() << '\n';
  kout << "baseAddress = " << reinterpret_cast<std::uint64_t>(baseAddress) << '\n';
  kassert(BaseAddress() == reinterpret_cast<std::uint64_t>(baseAddress)); // !
  kout << reinterpret_cast<std::uint64_t>(baseAddress) << '\n';
  kassert(Type() == 0b1001);
  kassert(IsPresent() == true);
  kassert(IsPrivilaged() == true);
  kassert(IsByteGranular() == true);
  kassert(IsBusy() == false);
  // checks for the reserved and zeros
  kassert(m_reserved0 == 0);
  kassert(m_res1_zeros == 0);
  kassert(m_res2 == 0);
}

TaskDescriptor::TaskDescriptor
  (
    SegmentAttributes::BaseLimit baselimit,
    SegmentAttributes::TaskAttributes taskatribs
  ) noexcept
{
  // Base Limit
  m_segdesc.SetBaseAddress(
      reinterpret_cast<std::uint64_t>(baselimit.base) & 0xFFFF'FFFF
  );
  m_segdesc.SetSegmentLimit(baselimit.limit);
  // Task Descriptor Specific
  SetAttributes(taskatribs);
  SetUpperBaseAddress(baselimit.base);
  SetReserved();
}

//---------------------------------------------------------------//
// Attribute Settors
//---------------------------------------------------------------//

void TaskDescriptor::SetUpperBaseAddress(void* baseAddress) noexcept{
  using u64 = std::uint64_t;
  static std::uint64_t ADDR_MASK = 0xFFFF'FFFFull << 32;
  m_base3 = static_cast<std::uint32_t>
    (
      (reinterpret_cast<u64>(baseAddress) & ADDR_MASK) >> 32
    );
}

void TaskDescriptor::SetAttributes(
    SegmentAttributes::TaskAttributes taskatribs
  ) noexcept
{
  m_segdesc.SetFlags({
      .isByteGranular = taskatribs.isByteGranular,
      // rest is fixed
      .db = false,
      .longModeSegment = false,
      .avl = false
  });

  std::uint8_t taskType = taskatribs.busy ? 0xb : 0x9;
  m_segdesc.SetAccessByte({
      .present = taskatribs.present,
      .privilaged = taskatribs.privilaged,
      .isSystemSegment = true,            
      .type = taskType
  });
}

void TaskDescriptor::SetReserved() noexcept{
  m_reserved0 = 0;
  m_res1_zeros = 0;
  m_res2 = 0;
}

//---------------------------------------------------------------//
// Attribute Querying
//---------------------------------------------------------------//

// Standard Segment Descriptor Attributes

[[nodiscard]] std::uint32_t
TaskDescriptor::SegmentLimit() const noexcept{
  return m_segdesc.SegmentLimit();
}

// Access Byte

[[nodiscard]] bool
TaskDescriptor::IsPresent() const noexcept{
  return m_segdesc.IsPresent();
}

[[nodiscard]] bool
TaskDescriptor::IsPrivilaged() const noexcept{
  return m_segdesc.IsPrivilaged();
}

[[nodiscard]] std::uint8_t
TaskDescriptor::Type() const noexcept{
  return m_segdesc.Type();
}

// Flags

[[nodiscard]] bool
TaskDescriptor::IsByteGranular() const noexcept{
  // set => measured in bytes
  // clear => measured in 4-KB units
  return m_segdesc.IsByteGranular();
}

// Task Segment Descriptor Attributes

[[nodiscard]] bool 
TaskDescriptor::IsBusy() const noexcept{
  return m_segdesc.Type() & 0x2;
}

[[nodiscard]] std::uint64_t
TaskDescriptor::BaseAddress() const noexcept{
  // in 64-bit mode the base address gets extended to 64 bits
  std::uint64_t address = (static_cast<std::uint64_t>(m_base3) << 32);
  address |= m_segdesc.BaseAddress();
  return address;
}

}
