#pragma once

#include <cstdint>

#include "memory/heap/allocator.hpp"

namespace Kernel{

  inline void* kmalloc(std::size_t bytes){
    namespace HeapAllocator = Mem::Heap::Allocator;
    return HeapAllocator::Allocate(bytes);
  }

}
