#ifndef  X86_64_INTERRUPTS_CONTROLLERS_APIC_UTIL_HPP
#define  X86_64_INTERRUPTS_CONTROLLERS_APIC_UTIL_HPP

// Just some example procedures to use

#include <cstdint>

#include "apic.hpp"

#include "x86_64/features/features.hpp"
#include "x86_64/features/msr.hpp"
#include "memory/address.hpp"
#include "drivers/serial/kostream.hpp"

namespace X8664::Interrupts{

void TryApic(){
  ApicController apic{};
  kout << intmode::hex << "apic base: " << apic.GetBaseAddress() << '\n';
  apic.EnableController();
  apic.Enumerate();
  kout << "apic id: " << apic.ReadId() << '\n';
  kout << "apic dfr: " << apic.ReadDfr() << '\n';
  kout << "apic svr: " << apic.ReadSpuriousVectorRegister() << '\n';
  apic.EnumerateVersion();
  apic.EnumerateLvt();
  apic.MaskLint0();
  apic.WriteLint1(apic.ReadLint1() | 0x10);
  apic.EnumerateLvt();
  //kout << "ia32_apic_base: " << read_ia32_apic_base() << '\n';
}

}

#endif
