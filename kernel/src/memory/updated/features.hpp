#ifndef MEMORY_FEATURES_HPP
#define MEMORY_FEATURES_HPP

// Functionalities to get the features of the memory mapping 
// at boot

#include <cstdint>

#include "address.hpp"
#include "include/kassert.hpp"
#include "serial/kostream.hpp"
#include "limine/requests.hpp"

namespace Mem{

void PrintMemoryMap();

std::size_t TotalUseableMemory();

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

#endif
