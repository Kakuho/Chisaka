#ifndef CPU_FEATURES_CPUID_HPP
#define CPU_FEATURES_CPUID_HPP

// Handles to processor identification and feature information, 
// generally requiring use of the cpuid instruction
//
// Reference: Intel vol3 CPUID

#include "cpuid.hpp"
#include "drivers/serial/kostream.hpp"
#include "primrose/static_array.hpp"

namespace X8664::Features{
  
std::uint32_t ReverseBytes(std::uint32_t src);

// cpuid.0

// should rename to VendorIdString like in the manual
Prim::StaticArray<char, 24> ManufacturerId();
void PrintManufacturerId();

// cpuid.1

bool Supportsx2APiC();      // interrupts
bool SupportsAPIC();        // interrupts

bool SupportsACPI();        // sw controlling hw

// cpuid.80000008

std::uint8_t PhysicalWidth();
std::uint8_t LinearWidth();
bool WbnoinvdAvailable();

} // namespace X8664::CpuId

#endif
