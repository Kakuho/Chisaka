#include "gdt_table.hpp"

namespace X8664{

//---------------------------------------------------------------//
//  Lifetime
//---------------------------------------------------------------//

GdtTable::GdtTable(
  SegmentDescriptor null, 
  SegmentDescriptor kernelCode, 
  SegmentDescriptor kernelData,
  SegmentDescriptor userCode,
  SegmentDescriptor userData,
  TaskDescriptor task
) noexcept: m_null{null},
            m_kernelCode{kernelCode},
            m_kernelData{kernelData},
            m_userCode{userCode},
            m_userData{userData},
            m_task{task}
{
  
}

//---------------------------------------------------------------//
//  Operational:
//    this should be the standard interface to the gdt
//---------------------------------------------------------------//

// observers to the segments themselves:

[[nodiscard]] SegmentDescriptor& 
GdtTable::NullSegment() noexcept
{
  return m_null;
}

[[nodiscard]] SegmentDescriptor& 
GdtTable::KernelCodeSegment() noexcept
{
  return m_kernelCode;
}

[[nodiscard]] SegmentDescriptor&
GdtTable::KernelDataSegment() noexcept
{
  return m_kernelData;
}

[[nodiscard]] SegmentDescriptor& 
GdtTable::UserCodeSegment() noexcept
{
  return m_userCode;
}

[[nodiscard]] SegmentDescriptor& 
GdtTable::UserDataSegment() noexcept
{
  return m_userData;
}

[[nodiscard]] TaskDescriptor&    
GdtTable::TaskSegment() noexcept
{
  return m_task;
}

// Immutable references 

[[nodiscard]] const SegmentDescriptor& 
GdtTable::NullSegment() const noexcept
{
  return m_null;
}

[[nodiscard]] const SegmentDescriptor& 
GdtTable::KernelCodeSegment() const noexcept
{
  return m_kernelCode;
}

[[nodiscard]] const SegmentDescriptor& 
GdtTable::KernelDataSegment() const noexcept
{
  return m_kernelData;
}

[[nodiscard]] const SegmentDescriptor& 
GdtTable::UserCodeSegment() const noexcept
{
  return m_userCode;
}

[[nodiscard]] const SegmentDescriptor& 
GdtTable::UserDataSegment() const noexcept
{
  return m_userData;
}

[[nodiscard]] const TaskDescriptor&    
GdtTable::TaskSegment() const noexcept
{
  return m_task;
}

} // namespace X8664
