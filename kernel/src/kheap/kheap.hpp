#pragma once

// the main interface to the kernel heap

#include <cstdint>

#include "concepts/allocator.hpp"
#include "concepts/kglobal.hpp"

namespace Chisaka{
  template<typename A>
    requires Concepts::HeapAllocator<A> && Concepts::KGlobalObject<A>
  class KHeap{
    public:
      static KHeap& Get(){ static KHeap kheap; return kheap;}

      void* Allocate(std::size_t bytes){ 
        return A::Get().AllocateObject(bytes);
      }

      void Deallocate(void* pobj){ 
        return A::Get().DeallocateObject(pobj);
      }

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
