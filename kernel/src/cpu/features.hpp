#ifndef FEATURES_HPP
#define FEATURES_HPP
// code to probe for cpu features
#include <cstdint>
#include <cpuid.h>
#include <kostream.hpp>

extern "C" std::uint64_t readcr4();
extern "C" std::uint64_t readcr3();

namespace features{
  void probeMemory();
  void probePhysicalWidth();
  void probeLinearWidth();
  void probex2apic();
  void probeLapic();

  // functions just to read control registers

  void probecr4();
  void probecr3();
}

#endif
