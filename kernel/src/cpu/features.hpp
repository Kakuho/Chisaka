#ifndef FEATURES_HPP
#define FEATURES_HPP
// code to probe for cpu features
#include <cstdint>
#include <cpuid.h>
#include <memory/memory.hpp>
#include <kostream.hpp>

extern "C"{
  std::uint64_t readcr0();
  void writecr0(std::uint32_t val);
  std::uint64_t readcr3();
  std::uint64_t readcr4();
}


namespace features{
  void probeMemory();

  // intel vol3a.4.1.1

  bool disablePaging();

  // intel vol3a.4.1.4
  
  void probePhysicalWidth();
  void probeLinearWidth();
  void probex2apic();
  void probeLapic();

  // intel vol3a.4.5 and vol3a.4.5.4

  mem::paddr64_t getPML4();

  // functions just to read control registers

  void probecr0();
  void probecr3();
  void probecr4();
}

#endif
