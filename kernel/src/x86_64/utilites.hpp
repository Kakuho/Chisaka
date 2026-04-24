#pragma once

// library for free standing cpu functions.

#include "types.hpp"
#include "drivers/serial/kostream.hpp"
#include "registers/control_registers.hpp"

namespace X8664{

[[noreturn]]
inline void HaltCatchFire(){
  asm("cli");
  for(;;){
    asm("hlt");
  }
}

inline void InvalidateTlb(){
  // intel 4.10.4.1
  // move to cr0 causes an invalidation
  volatile std::uint64_t cr0 = readcr0();
  cr0 &= 0xEFFF'FFFF;
  writecr0(cr0);
  cr0 |= 0x8000'0000;
  writecr0(cr0);
}

struct PageIndices{
    std::uint16_t pm4le;
    std::uint16_t pdpte;
    std::uint16_t pde;
    std::uint16_t pt;
};


Chisaka::VirtAddr KVirtFromPageIndex(
    PageIndices&& indices,
    std::uint16_t offset
);

/*
PageIndices ExtractPageIndicies(Chisaka::VirtAddr vaddr);
*/

void PrintPageIndicies(Chisaka::VirtAddr vaddr);

}
