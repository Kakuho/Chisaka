#pragma once

#include "memory/page/freelist.hpp"
#include "memory/page/page_allocator.hpp"

namespace Kernel{
  inline void* palloc(){
    return Mem::PageAllocator::Allocate();
  }

  inline void* palloc(unsigned pages){
    return Mem::PageAllocator::Allocate(pages);
  }

  inline void pfree(void* pagebase){
    return Mem::PageAllocator::Deallocate(pagebase);
  }

} // namespace Kernel
