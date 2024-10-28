#ifndef CPU_FEATURES_CPUID_HPP
#define CPU_FEATURES_CPUID_HPP

// Handles to processor specific information, requiring the use 
// of the cpuid instruction
//
// Reference: Intel vol3 

#include "cpuid_asm.hpp"
#include "serial/kostream.hpp"
#include "primrose/static_array.hpp"

namespace X8664::CpuId{
  
std::uint32_t ReverseBytes(std::uint32_t src);

Prim::StaticArray<char, 24> ManufacturerId();
void PrintManufacturerId();

} // namespace X8664::CpuId

#endif
