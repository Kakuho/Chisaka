#pragma once

// Handles to processor identification and feature information, 
// generally requiring use of the cpuid instruction
//
// Reference: Intel vol3 CPUID

#include "cpuid.hpp"
#include "msr.hpp"
#include "drivers/serial/kostream.hpp"
#include "aii/array.hpp"
#include "types.hpp"

namespace X8664::Features{
  
// Vendor Identification

Aii::Array<char, 24> ManufacturerId();
std::uint32_t ReverseBytes(std::uint32_t src);
void PrintManufacturerId();

// APIC

bool SupportsAPIC();
Chisaka::PhysAddr64 GetApicBase();

void EnableApic();
void DisableApic();

// x2APIC

bool Supportsx2APiC();
void Disablex2Apic();

// ACPI

bool SupportsACPI();

// Memory Width

std::uint8_t PhysicalWidth();
std::uint8_t LinearWidth();
bool WbnoinvdAvailable();

} // namespace X8664::Features
