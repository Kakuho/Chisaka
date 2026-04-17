#pragma once

// main frontend to the slab heap allocator

#include <cstdint>
#include <new>

#include "kassert.hpp"
#include "drivers/serial/kostream.hpp"

#include "list_descriptor.hpp"
#include "buffer.hpp"

#include "aii/array.hpp"

namespace Chisaka::Slab::v2{

class Allocator{
  using PageAllocation_fn = void*(*)();
  using PageDeallocation_fn = void(*)(void*);

  public:
    static Allocator& Get(){ static Allocator alc; return alc;}
    void Init(PageAllocation_fn alloc, PageDeallocation_fn dealloc);

    Buffer& GetBuffer(std::uint16_t size);
    void AddList(ListDescriptor* list);

    // heap management w.r.t types
    template<typename T>  
    T* New(){ 
      T* alloc = static_cast<T*>(AllocateObject(sizeof(T)));
      alloc = new(alloc) T;
      return alloc;
    }

    template<typename T>  
    void Delete(T* pobj){ 
      DeallocateObject(pobj, sizeof(T));
    }

  private:
    void InitBuffers();

    ListDescriptor* AllocateList(void* pagebase, std::uint16_t bufferSize);
    void DeallocateList(ListDescriptor* list);

    void* AllocateObject(std::size_t bytes);
    void DeallocateObject(void* pobj, std::size_t bytes);

    static constexpr std::size_t NbytesToPow2(std::size_t nbytes){
      // ref: https://graphics.stanford.edu/%7Eseander/bithacks.html#RoundUpPowerOf2
      nbytes--;
      nbytes |= nbytes >> 1;
      nbytes |= nbytes >> 2;
      nbytes |= nbytes >> 4;
      nbytes |= nbytes >> 8;
      nbytes |= nbytes >> 16;
      nbytes |= nbytes >> 32;
      nbytes++;
      return nbytes;
    }

    static constexpr int Pow2ToIndex(std::size_t pow2) noexcept{ 
      // would be nice to have a std::expected here
      switch(pow2){
        case 2: case 4: case 8: return 0;
        case 16: return 1;
        case 32: return 2;
        case 64: return 3;
        case 128: return 4;
        case 256: return 5;
        case 512: return 6;
        case 1024: return 7;
        case 2048: return 8;
        default: kassert(false && "Pow2ToIndex::pow2 is not a power of 2"); return -1;
      }
    }

  private:
    PageAllocation_fn m_palloc;
    PageDeallocation_fn m_pdealloc;
    Aii::Array<Buffer,  9> m_buffers;
};

}
