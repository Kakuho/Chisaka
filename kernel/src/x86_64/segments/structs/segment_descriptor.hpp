#ifndef SEGMENT_DESCRIPTOR_HPP
#define SEGMENT_DESCRIPTOR_HPP

// class to represent segment descriptors
// Reference: intel docs vol3a 3.4.5
//
// N.B our kernel only uses 2 privilage level - level 0 and level 3

#include <cstdint>
#include <cassert>

#include "attributes.hpp"
#include "memory/old/memory.hpp"
#include "lib/kassert.hpp"
#include "drivers/serial/kostream.hpp"

namespace X8664{

struct __attribute__((packed)) SegmentDescriptor{
  friend class TaskDescriptor;
  public:
    //---------------------------------------------------------------//
    // Lifetime:
    //---------------------------------------------------------------//
    
    SegmentDescriptor() noexcept = default;

    SegmentDescriptor(
        void* baseAddress, 
        std::uint16_t limit0,
        std::uint8_t accessByte,
        std::uint8_t flagsLimit1
    ) noexcept;

    SegmentDescriptor(
        SegmentAttributes::BaseLimit baselimit,
        SegmentAttributes::AccessByte&& accessbyte,
        SegmentAttributes::Flags&& flags
    ) noexcept;

    ~SegmentDescriptor() = default;

    //---------------------------------------------------------------//
    // Segment Attributes Modifiers:
    //---------------------------------------------------------------//

    void SetBaseAddress(void* baseAddress) noexcept;
    void SetSegmentLimit(std::uint32_t limit) noexcept;
    void SetBaseLimit(
        SegmentAttributes::BaseLimit& baseLimit) noexcept;
    void SetAccessByte(
        SegmentAttributes::AccessByte& accessbyte) noexcept;
    void SetAccessByte(
        SegmentAttributes::AccessByte&& accessbyte) noexcept;
    void SetFlags(SegmentAttributes::Flags& flags) noexcept;
    void SetFlags(SegmentAttributes::Flags&& flags) noexcept;

  private:
    void SetBaseAddress(std::uint32_t baseAddress) noexcept
    {SetBaseAddress(reinterpret_cast<void*>(baseAddress));}

    //---------------------------------------------------------------//
    //  Attribute Querying:
    //---------------------------------------------------------------//

  public:
    [[nodiscard]] std::uint32_t SegmentLimit() const noexcept;
    [[nodiscard]] std::uint32_t BaseAddress() const noexcept;
    // Access Byte
    [[nodiscard]] bool IsPresent() const noexcept;
    [[nodiscard]] bool IsPrivilaged() const noexcept;
    // Segment Type
    [[nodiscard]] bool IsSystemSegment() const noexcept;
    [[nodiscard]] bool IsDataSegment() const noexcept;
    [[nodiscard]] bool IsCodeSegment() const noexcept;
    [[nodiscard]] std::uint8_t Type() const noexcept;
    // Flags
    [[nodiscard]] bool IsByteGranular() const noexcept;
    [[nodiscard]] bool IsDB() const noexcept;
    [[nodiscard]] bool IsLongCodeSegment() const noexcept;

  public:
    //---------------------------------------------------------------//
    //  I made these public in case the kernel explicitly needs to 
    //  access its underlying representation
    //---------------------------------------------------------------//
      
    std::uint16_t m_limit0;
    std::uint16_t m_base0;
    std::uint8_t  m_base1;
    // accessbyte composed of:      type, s, dpl, p
    std::uint8_t  m_accessbyte;   
    // flag and limit composed of:  avl, l, d/b and g
    std::uint8_t  m_flags_limit1;  
    std::uint8_t  m_base2;
};

} // namespace X8664

#endif
