#ifndef INTERRUPTS_DESCRIPTORS_HPP
#define INTERRUPTS_DESCRIPTORS_HPP

// class definitions for descriptors relating to interrupts including:
//  IDT Descriptor
//  IDT Gate Descriptor

#include <cstdint>

#include "primrose/static_array.hpp"

namespace X8664{

  // https://wiki.osdev.org/Interrupt_Descriptor_Table
  // intel manual vol3a ??
  struct [[gnu::packed]] IdtDescriptor{
    std::uint16_t limit;
    std::uint64_t offset;
  };

  struct [[gnu::packed]] IdtGateDescriptor{
    std::uint16_t offset0;
    std::uint16_t segment_selector;   // isnt this ignored in 64 bit mode??
    std::uint16_t p_dpl_gatetype_ist; // bit packed word
    std::uint16_t offset1;
    std::uint32_t offset2;
    std::uint32_t rsv;
  };

} // namespace X8664

#endif
