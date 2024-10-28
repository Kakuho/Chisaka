#include "cpuid.hpp"

namespace X8664::CpuId{

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

Prim::StaticArray<char, 24> ManufacturerId(){
  Prim::StaticArray <char, 24> manuid{};
  std::uint64_t base = cpuid_0();
  std::uint32_t ebx = *(reinterpret_cast<std::uint32_t*>(base-4));
  std::uint32_t ecx = *(reinterpret_cast<std::uint32_t*>(base-8));
  std::uint32_t edx = *(reinterpret_cast<std::uint32_t*>(base-12));
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
  std::uint32_t ebx = *(reinterpret_cast<std::uint32_t*>(base-4));
  std::uint32_t ecx = *(reinterpret_cast<std::uint32_t*>(base-8));
  std::uint32_t edx = *(reinterpret_cast<std::uint32_t*>(base-12));
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
  for(int i = 0; i < 4; i++){
    kout << static_cast<char>((ebx >> i*8) & 0xFF);
  }
  for(int i = 0; i < 4; i++){
    kout << static_cast<char>((edx >> i*8) & 0xFF);
  }
  for(int i = 0; i < 4; i++){
    kout << static_cast<char>((ecx >> i*8) & 0xFF);
  }
  kout << ManufacturerId().m_buffer;
  kout << '\n';
}

} // namespace X8664::CpuId
