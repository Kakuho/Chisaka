#pragma once

#include "freelist.hpp"

#include "memmap/memory_map.hpp"

namespace Mem::PageAllocator{
  using namespace Chisaka;
  namespace Policy = Mem::Page::Freelist;

  inline void Initialise(){
    Policy::Initialise(MemoryMap::Get());
  }

  inline void Initialise(const MemoryMap& memmap){
    Policy::Initialise(MemoryMap::Get());
  }
  
  inline void* Allocate(){
    return Policy::AllocatePage();
  }

  inline void* Allocate(unsigned pages){
    return Policy::AllocatePages(pages);
  }

  inline void Deallocate(void* base){
    Policy::DeallocPage(base);
  }
}
