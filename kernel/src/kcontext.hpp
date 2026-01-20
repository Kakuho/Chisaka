#pragma once

// namespace to store and configure your global kernel managers

#include <cstddef>
#include "limine/utility.hpp"
#include "types.hpp"

#include "memmap/memory_map.hpp"
#include "ram/page_allocators/freelist.hpp"
#include "ram/ram.hpp"

#include "kheap/slab/allocator.hpp"
#include "kheap/kheap.hpp"

namespace Chisaka{
  class KContext{
    public: 
      static constexpr std::uint16_t PAGE_SIZE = 0x1000;

      // set up of the global kernel objects

      using PageAllocator = PageAllocators::Freelist<MemoryMap>;
      using Ram = Chisaka::Ram<PageAllocator>;

      using HeapAllocator = Slab::Allocator;
      using KHeap = KHeap<HeapAllocator>;

    public:
      static KContext& Get(){ static KContext g; return g;}

      void InitKContext();

      PhysAddr KernelPhysAddr();
      VirtAddr KernelVirtAddr();

      VirtAddr PhysToVirtAddr(PhysAddr paddr);
      PhysAddr VirtToPhysAddr(VirtAddr vaddr);

    private:
      VirtAddr kernelBase;
      size_t hhdmOffset;
  };
}
