#include "features.hpp"

namespace X8664::Features{

std::uint32_t ReverseBytes(std::uint32_t src){
  std::uint32_t oriented = src & 0xFF;
  oriented <<= 8;
  oriented |= (src >> 8) & 0xFF;
  oriented <<= 8;
  oriented |= (src >> 16) & 0xFF;
  oriented <<= 8;
  oriented |= (src >> 24) & 0xFF;
  return oriented;
}

Aii::Array<char, 24> ManufacturerId(){
  Aii::Array <char, 24> manuid{};
  std::uint64_t base = cpuid_0();
  std::uint32_t ebx = *(reinterpret_cast<std::uint32_t*>(base-8));
  std::uint32_t ecx = *(reinterpret_cast<std::uint32_t*>(base-12));
  std::uint32_t edx = *(reinterpret_cast<std::uint32_t*>(base-16));
  for(int i = 0; i < 4; i++){
    manuid[i] =  static_cast<char>((ebx >> i*8) & 0xFF);
  }
  for(int i = 0; i < 4; i++){
    manuid[i+4] = static_cast<char>((edx >> i*8) & 0xFF);
  }
  for(int i = 0; i < 4; i++){
    manuid[i+8] = static_cast<char>((ecx >> i*8) & 0xFF);
  }
  return manuid;
}

void PrintManufacturerId(){
  std::uint64_t base = cpuid_0();
  std::uint32_t ebx = *(reinterpret_cast<std::uint32_t*>(base-8));
  std::uint32_t ecx = *(reinterpret_cast<std::uint32_t*>(base-12));
  std::uint32_t edx = *(reinterpret_cast<std::uint32_t*>(base-16));

  kout << intmode::hex;
  kout << "ebx: " << ebx << '\n';
  kout << "ecx: " << ecx << '\n';
  kout << "edx: " << edx << '\n';
  // now we form the machine string
  kout << "orientating" << '\n';
  std::uint32_t ebxoriented = ReverseBytes(ebx);
  std::uint32_t ecxoriented = ReverseBytes(ecx);
  std::uint32_t edxoriented = ReverseBytes(edx);
  kout << intmode::hex;
  kout << "ebx: " << ebxoriented << '\n';
  kout << "ecx: " << ecxoriented << '\n';
  kout << "edx: " << edxoriented << '\n';
  // now we start printing the string
  kout << "Manufacturer ID: ";
  kout << ManufacturerId().m_buffer;
  kout << '\n';
}

// cpuid.1

bool Supportsx2APiC(){
  std::uint64_t base = cpuid_1();
  std::uint32_t ecx = *(reinterpret_cast<std::uint32_t*>(base-12));
  return (ecx >> 22) & 1;
}

bool SupportsAPIC(){
  std::uint64_t base = cpuid_1();
  std::uint32_t edx = *(reinterpret_cast<std::uint32_t*>(base-16));
  return (edx >> 9) & 1;
}

bool SupportsACPI(){
  std::uint64_t base = cpuid_1();
  std::uint32_t edx = *(reinterpret_cast<std::uint32_t*>(base-16));
  return (edx >> 22) & 1;
}

// CPUID.80000008

std::uint8_t PhysicalWidth(){
  std::uint64_t base = cpuid_80000008();
  std::uint32_t eax = *(reinterpret_cast<std::uint32_t*>(base-4));
  return eax & 0xFF;
}

std::uint8_t LinearWidth(){
  std::uint64_t base = cpuid_80000008();
  std::uint32_t eax = *(reinterpret_cast<std::uint32_t*>(base-4));
  return (eax >> 8) & 0xFF;
}

bool WbnoinvdAvailable(){
  // cache?
  std::uint64_t base = cpuid_80000008();
  std::uint32_t ebx = *(reinterpret_cast<std::uint32_t*>(base-8));
  return ebx & 0x200;
}

} // namespace X8664::CpuId
