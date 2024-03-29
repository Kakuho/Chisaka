#include "features.hpp"

void features::probeMemory(){
  //cpuid normally references 32 bit registers eax, ebx ect.
  std::uint32_t eax, ebx, ecx, edx;
  // probe physaddr
  __get_cpuid(0x80000008, &eax, &ebx, &ecx, &edx);
  std::uint8_t width = eax & 0b1111'1111;
  kout << intmode::dec << "MAXPHYSADDR = " << width << '\n';
}

void features::probeLinearWidth(){
  //cpuid normally references 32 bit registers eax, ebx ect.
  std::uint32_t eax, ebx, ecx, edx;
  // probe physaddr
  __get_cpuid(0x80000008, &eax, &ebx, &ecx, &edx);
  std::uint8_t linearWidth = (eax & (0b1111'1111 << 8)) >> 8;
  kout << intmode::dec << "LinearWidth = " << linearWidth << '\n';
  // is 48 :D
}

void features::probex2apic(){
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

void features::probeLapic(){
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

void features::probecr4(){
  std::uint64_t result = readcr4();
  kout << intmode::dec << "cr4 = " << result << intmode::bin << ' ' << result << 
       '\n';
}

// 4.5.2 details the fields of cr3
void features::probecr3(){
  std::uint64_t result = readcr3();
  kout << "cr3 =" 
       << intmode::dec << ' '  << result << ' '
       << intmode::hex << "0x" << result << ' ' 
       << intmode::bin << "0b" << result 
       <<  '\n';
}
