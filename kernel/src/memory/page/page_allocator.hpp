#pragma once

#include "freelist.hpp"
#include "./../memory_map/memory_map_descriptor.hpp"

namespace Mem::PageAllocator{
  namespace Policy = Mem::Page::Freelist;

  inline void Initialise(){
    MemoryMapDescriptor memmap{};
    Policy::Initialise(memmap);
  }

  inline void Initialise(const MemoryMapDescriptor& memmap){
    Policy::Initialise(memmap);
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
