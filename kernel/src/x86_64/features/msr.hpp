#ifndef X8664_MSR_HPP
#define X8664_MSR_HPP

#include <cstdint>

extern "C"{
  std::uint64_t read_ia32_apic_base();
  void write_ia32_apic_base(std::uint32_t edx, std::uint32_t eax);
}

#endif
