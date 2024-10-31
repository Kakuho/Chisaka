#ifndef GDT_TABLE_HPP
#define GDT_TABLE_HPP

// Class to represent the GDT table
//
// This kernel uses the basic flat model, where user mode and kernel mode
// has access to the entire linear address space
//
// Reference: Intel vol3-3.2.1

#include "segment_descriptor.hpp"
#include "task_descriptor.hpp"

namespace X8664{

struct __attribute__((packed)) GdtTable{
  public:
    //---------------------------------------------------------------//
    //  Lifetime
    //---------------------------------------------------------------//

    GdtTable() = default;

    GdtTable(
        SegmentDescriptor null, 
        SegmentDescriptor kernelCode, 
        SegmentDescriptor kernelData,
        SegmentDescriptor userCode,
        SegmentDescriptor userData,
        TaskDescriptor task
    ) noexcept;

    //---------------------------------------------------------------//
    //  Operational:
    //    this should be the standard interface to the gdt
    //---------------------------------------------------------------//

    [[nodiscard]] constexpr 
    std::uint64_t Size() const noexcept
    { return sizeof(GdtTable); }

    // observers to the segments themselves:
    [[nodiscard]] SegmentDescriptor&  NullSegment()       noexcept;
    [[nodiscard]] SegmentDescriptor&  KernelCodeSegment() noexcept;
    [[nodiscard]] SegmentDescriptor&  KernelDataSegment() noexcept;
    [[nodiscard]] SegmentDescriptor&  UserCodeSegment()   noexcept;
    [[nodiscard]] SegmentDescriptor&  UserDataSegment()   noexcept;
    [[nodiscard]] TaskDescriptor&     TaskSegment()       noexcept;

    // immutable references
    [[nodiscard]] const SegmentDescriptor& 
    NullSegment() const noexcept;

    [[nodiscard]] const SegmentDescriptor& 
    KernelCodeSegment() const noexcept;

    [[nodiscard]] const SegmentDescriptor& 
    KernelDataSegment() const noexcept;

    [[nodiscard]] const SegmentDescriptor& 
    UserCodeSegment() const noexcept;

    [[nodiscard]] const SegmentDescriptor& 
    UserDataSegment() const noexcept ;

    [[nodiscard]] const TaskDescriptor& TaskSegment() const noexcept;

  public:
    //---------------------------------------------------------------//
    //  Underlying Representation:
    //    it is public in case the kernel requires access
    //---------------------------------------------------------------//

    SegmentDescriptor m_null;
    SegmentDescriptor m_kernelCode;
    SegmentDescriptor m_kernelData;
    SegmentDescriptor m_userCode;
    SegmentDescriptor m_userData;
    TaskDescriptor    m_task;
};

}

#endif
