#pragma once 

// Functionalities to get the features of the memory mapping 
// at boot

#include <cstdint>

#include "address.hpp"
#include "kassert.hpp"
#include "drivers/serial/kostream.hpp"
#include "limine/utility.hpp"

namespace Mem{

void PrintMemoryMap();

std::size_t TotalUseableMemory();

template <std::size_t PageSize>
[[nodiscard]] constexpr std::size_t TotalUseablePageFrames(){
  // there is an extra 1 for padding / to ensure the division does 
  // not lose the fractional part.
  return TotalUseableMemory() / PageSize;
}

//-------------------------------------------------------------
//  Probers:
//    these functions are more like run time examples
//-------------------------------------------------------------

// Probing in this case means writting and reading from memory address

void ProbeMemory(std::size_t memIndex);
void ProbeUpperLimit();
void ProbeLowerLimit();

//-------------------------------------------------------------
// DO NOT USE
//-------------------------------------------------------------

[[noreturn]] void CorruptMemory();

} // namespace Mem
