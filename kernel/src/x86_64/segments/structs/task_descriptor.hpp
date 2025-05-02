#pragma once

// Class to represent the Task State Segment Descriptor
// Reference: intel vol3.8.2.3

#include <cassert>

#include "segment_descriptor.hpp"
#include "attributes.hpp"
#include "kassert.hpp"
#include "drivers/serial/kostream.hpp"

namespace X8664{

struct [[gnu::packed]] TaskDescriptor{
  public:
    //---------------------------------------------------------------//
    // Lifetime
    //---------------------------------------------------------------//

    TaskDescriptor() = default;

    TaskDescriptor(
        void* baseAddress, 
        std::uint32_t limit
    ) noexcept;


    TaskDescriptor(
        SegmentAttributes::BaseLimit baselimit,
        SegmentAttributes::TaskAttributes taskatbs
    ) noexcept;

    ~TaskDescriptor() = default;

    //---------------------------------------------------------------//
    // Attribute Querying:
    //---------------------------------------------------------------//

    // Standard Segment Descriptor Attributes

    [[nodiscard]] std::uint32_t SegmentLimit() const noexcept;
    // Access Byte
    [[nodiscard]] bool IsPresent() const noexcept;
    [[nodiscard]] bool IsPrivilaged() const noexcept;
    [[nodiscard]] std::uint8_t Type() const noexcept;
    // Flags
    [[nodiscard]] bool IsByteGranular() const noexcept;

    // Task Segment Descriptor Attributes

    [[nodiscard]] bool IsBusy() const noexcept;
    [[nodiscard]] std::uint64_t BaseAddress() const noexcept;


    //---------------------------------------------------------------//
    // Attribute Setters:
    //    Can mainly use the segment descriptor functions here,
    //    but some data is Task Descriptor specific
    //---------------------------------------------------------------//

    void SetSegmentDescriptor() noexcept;
    void SetUpperBaseAddress(void* baseAddress) noexcept;
    void SetAttributes(
        SegmentAttributes::TaskAttributes taskatribs) noexcept;
    void SetReserved() noexcept;

  public:
    // A Task Descriptor occupies two spaces on the GDT descriptor table
    //    the first is a normal segment descriptor
    //    the second is task descriptor specific data
     
    // I made the underlying representation public in case the kernel
    // needs to access it
    
    SegmentDescriptor m_segdesc;
    std::uint32_t m_base3;
    std::uint8_t m_reserved0;
    std::uint8_t m_res1_zeros;
    std::uint16_t m_res2;
};

static_assert(sizeof(TaskDescriptor) == 16);

}
