#pragma once

// Class to represent the Idt Register Descriptor. 
//
// Reference: Intel vol3-2.4.3

#include <cstdint>
#include <cassert>

#include "kassert.hpp"
#include "idt_table.hpp"

extern "C" void load_idtr(void* address);

namespace X8664{

class [[gnu::packed]] IdtDescriptor{
  public:
    //---------------------------------------------------------------//
    //  Lifetime
    //---------------------------------------------------------------//
    
    IdtDescriptor() noexcept = default;

    IdtDescriptor(IdtTable& table) noexcept;

    IdtDescriptor(
        std::uint16_t limit,
        std::uint64_t offset
    ) noexcept;

    IdtDescriptor(
        std::uint16_t limit,
        void* offset
    ) noexcept: IdtDescriptor(limit, reinterpret_cast<std::uint64_t>(offset))
    {}


    //---------------------------------------------------------------//
    // Operational
    //---------------------------------------------------------------//

    void SwitchIdt(IdtTable& idt) noexcept;
    void LoadCurrent() noexcept { load_idtr(this); }
    void SwitchLoad(IdtTable& table) noexcept;

    //---------------------------------------------------------------//
    // Queries
    //---------------------------------------------------------------//

    std::uint16_t Limit()   const { return m_limit;} 
    std::uint64_t Offset()  const { return m_offset;}

  public:
    std::uint16_t m_limit;
    std::uint64_t m_offset;
};

// size limitation

static_assert(sizeof(IdtDescriptor) == 10);

} // namespace X8664
