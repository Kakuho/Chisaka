#pragma once

// library for free standing cpu functions.

#include "types.hpp"
#include "drivers/serial/kostream.hpp"

namespace X8664{

[[noreturn]]
inline void HaltCatchFire(){
  asm("cli");
  for(;;){
    asm("hlt");
  }
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

PageIndices ExtractPageIndicies(Chisaka::VirtAddr vaddr);
void PrintPageIndicies(Chisaka::VirtAddr vaddr);

}
