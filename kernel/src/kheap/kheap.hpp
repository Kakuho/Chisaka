#pragma once

// the main interface to the kernel heap

#include <cstdint>
#include "concepts/allocator.hpp"

namespace Chisaka{
  template<typename A>
    requires Concepts::HeapAllocator<A>
  class KHeap{
    void* Allocate(std::size_t bytes);
    void Deallocate(void* pobj);

    // heap management w.r.t types
    template<typename T>
    T* New(){
      T* alloc = static_cast<T*>(Allocate(sizeof(T)));
      alloc = new(alloc) T;
      return alloc;
    }

    template<typename T>
    void Delete(T* pobj){
      Deallocate(pobj);
    }
  };
}
