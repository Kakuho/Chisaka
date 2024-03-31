#include <cstdint>

namespace idt{
  // https://wiki.osdev.org/Interrupt_Descriptor_Table
  // intel manual vol3a ??
  struct __attribute__((packed)) idtDescriptor{
    std::uint16_t size;
    std::uint64_t offset;
  };

  struct __attribute__((packed)) idtGate{
    std::uint16_t offset;
    std::uint16_t segment_selector;   // isnt this ignored in 64 bit mode??
    std::uint16_t p_dpl_gatetype_ist; // bit packed word
    std::uint16_t offset2;
    std::uint32_t offset3;
    std::uint32_t rsv;
  };

  extern idtGate idt_table[256];

}
