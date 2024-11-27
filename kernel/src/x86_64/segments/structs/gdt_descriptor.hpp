#ifndef GDT_DESCRIPTOR_HPP
#define GDT_DESCRIPTOR_HPP

// Data structure to represent the values required by the x86 processor 
// in order to load a new gdt table into the Gdt register
//
// As such, it should be thought of the Gdt register Descriptor
//
// Reference: Intel vol3-2.4.1

#include <cstdint>

#include "gdt_table.hpp"
#include "attributes.hpp"

extern "C" void load_gdtr(void* address);

namespace X8664{


struct [[gnu::packed]] GdtDescriptor{
  public:
    //---------------------------------------------------------------//
    //  Lifetime
    //---------------------------------------------------------------//

    GdtDescriptor() noexcept = default;

    GdtDescriptor(
        std::uint16_t limit, 
        std::uint64_t offset
    ) noexcept;

    GdtDescriptor(
        SegmentAttributes::LimitOffsetArgs args
    ) noexcept;

    GdtDescriptor(GdtTable& gdtTable) noexcept;

    //---------------------------------------------------------------//
    // Operational
    //---------------------------------------------------------------//

    void SwitchGdt(GdtTable& gdtTable) noexcept;

    //  asm procedure to load the gdt
    void LoadCurrent() noexcept
    { load_gdtr(this); }

    //  asm procedure to load the gdt
    void SwitchLoad(GdtTable& gdtTable) noexcept;

    //---------------------------------------------------------------//
    // Queries
    //---------------------------------------------------------------//
    
    std::uint16_t Limit() const noexcept{ return m_limit;}
    std::uint64_t Offset() const noexcept{ return m_offset;}

  private:
    std::uint16_t m_limit;
    std::uint64_t m_offset;
};

// the descriptor should be 80 bits [16 + 64] in 
// 64-bit mode:

static_assert(sizeof(GdtDescriptor) == 10); 

} // namespace X8664

#endif
