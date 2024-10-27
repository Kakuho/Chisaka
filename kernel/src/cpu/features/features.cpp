#include "features.hpp"

void x8664::features::probePhysicalWidth(){
  //cpuid normally references 32 bit registers eax, ebx ect.
  std::uint32_t eax, ebx, ecx, edx;
  // probe physaddr
  __get_cpuid(0x8000'0008, &eax, &ebx, &ecx, &edx);
  std::uint8_t width = eax & 0b1111'1111;
  kout << intmode::dec << "MAXPHYSADDR = " << width << '\n';
}

bool x8664::features::disablePaging(){
  std::uint32_t newcr0 = readcr0();
  newcr0 &= 1 << 31;
  writecr0(newcr0);
  return true;
}

void x8664::features::probeLinearWidth(){
  //cpuid normally references 32 bit registers eax, ebx ect.
  std::uint32_t eax, ebx, ecx, edx;
  // probe physaddr
  __get_cpuid(0x80000008, &eax, &ebx, &ecx, &edx);
  std::uint8_t linearWidth = (eax & (0b1111'1111 << 8)) >> 8;
  kout << intmode::dec << "LinearWidth = " << linearWidth << '\n';
  // is 48 :D
}

void x8664::features::probex2apic(){
  std::uint32_t eax, ebx, ecx, edx;
  // probe x2apic
  __get_cpuid(0x01, &eax, &ebx, &ecx, &edx);
  bool is_x2apic = ecx & (0x01 << 21);
  if(is_x2apic){
    kout << "supports x2APIC\n";
  }
  else{
    kout << "does not support x2APIC\n";
  }
}

void x8664::features::probeLapic(){
  std::uint32_t eax = 1;
  std::uint32_t ebx, ecx, edx;
  // probe x2apic
  __cpuid(0x01, eax, ebx, ecx, edx);
  bool is_lapic = edx & (0x01 << 21);
  if(is_lapic){
    kout << "supports x2APIC\n";
  }
  else{
    kout << "does not support x2APIC\n";
  }
}

memory::paddr64_t x8664::features::getPML4(){
  // the physical address of the pml4 table is located in bits 54:12 of cr3
  std::uint64_t cr3 = readcr3();
  std::uint64_t mask = (0xFFFFFFF << 12);
  memory::paddr64_t addr = (cr3 & mask) >> 12;
  return addr;
} 

void x8664::features::probecr4(){
  std::uint64_t result = readcr4();
  kout << intmode::dec << "cr4 = " << result << intmode::bin << ' ' << result << 
       '\n';
}

void x8664::features::probecr0(){
  std::uint64_t result = readcr0();
  kout << "cr0 =" 
       << intmode::dec << ' '  << result << ' '
       << intmode::hex << "0x" << result << ' ' 
       << intmode::bin << "0b" << result 
       <<  '\n';
}

// 4.5.2 details the fields of cr3
void x8664::features::probecr3(){
  std::uint64_t result = readcr3();
  kout << "cr3 =" 
       << intmode::dec << ' '  << result << ' '
       << intmode::hex << "0x" << result << ' ' 
       << intmode::bin << "0b" << result 
       <<  '\n';
}
