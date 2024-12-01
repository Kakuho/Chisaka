#ifndef MEMORY_HEAP_SLAB_CACHE_DESCRIPTOR_HPP
#define MEMORY_HEAP_SLAB_CACHE_DESCRIPTOR_HPP

//  Cache Descriptors and related functionalities
//
//
//  Each Cache Desriptor has the head of a doubly linked list of its slab 
//  descriptors

#include <cassert>
#include <utility>

#include "slab_descriptor.hpp"
#include "slab_list.hpp"
#include "./../../physical/freelist.hpp"

namespace Mem::Heap::Slab::T{

template<typename T>
class SlabDescriptor;

template<typename T>
class CacheDescriptor{
  using PageAllocator_t = Mem::Phys::FreeList;
  using CnEntry_t = SlabDescriptor<T>;
  using SlabCacheEntry_t = SlabDescriptor<void*>;
  using SlabCache_t = CacheDescriptor<SlabCacheEntry_t>;

  public:
    //-------------------------------------------------------------
    //  Lifetime
    //-------------------------------------------------------------

    explicit CacheDescriptor();

    //-------------------------------------------------------------
    // Slab Management
    //-------------------------------------------------------------

    [[nodiscard]] SlabDescriptor<T>* 
    AllocateSlab(PageAllocator_t& pageAllocator, SlabCache_t& slabCache) noexcept;

    void FreeSlab(SlabDescriptor<T>* pslab);
    void DestroySlabs();  // iteratively delete all slabs

    SlabDescriptor<T>* PartialSlabsHead(){ return m_partialSlabs.Head();}
    SlabDescriptor<T>* FullSlabsHead(){ return m_fullSlabs.Head();}
    SlabDescriptor<T>* EmptySlabsHead(){ return m_partialSlabs.Head();}
    
    //-------------------------------------------------------------
    // Object Management
    //-------------------------------------------------------------

    template<typename ...Args>
    T* AllocateObject(Args&&... args){
      if(!m_partialSlabs.IsEmpty()){
        m_partialSlabs.Head()->Allocate(std::forward<Args>(args)...);
      }
      else if(!m_freeSlabs.IsEmpty()){
        m_freeSlabs.Head()->Allocate(std::forward<Args>(args)...);
      }
      else{
        AllocateSlab();
        m_freeSlabs.Head()->Allocate(std::forward<Args>(args)...);
      }
    }

    void FreeObject(T* pobject);

  private:
    SlabList<T> m_partialSlabs;
    SlabList<T> m_fullSlabs;
    SlabList<T> m_freeSlabs;
    // used for quick querying
    std::size_t objectsPerSlab;
    std::size_t freeObjects;    
};

//-------------------------------------------------------------
//  Lifetime
//-------------------------------------------------------------

template<typename T>
CacheDescriptor<T>::CacheDescriptor(){
}

//-------------------------------------------------------------
// Slab Management 
//-------------------------------------------------------------

template<typename T>
SlabDescriptor<T>* 
CacheDescriptor<T>::AllocateSlab(PageAllocator_t& pageAllocator, SlabCache_t& slabCache)
noexcept{
  void* base = pageAllocator.AllocatePage();
  CnEntry_t* slab = slabCache.AllocateObject(base);
  if constexpr(sizeof(T) < 0x1000){
    slab->InitialiseSmall();
  }
  else{
    slab->InitialiseLarge();
  }
  m_freeSlabs.AddBack(slab);
}

template<typename T>
void CacheDescriptor<T>::FreeSlab(SlabDescriptor<T>* pslab){
  //  Procedure:
  //    * Call Destructor on the objects of the Slab
  //    * Release the page frame back to the page frame allocator
  //    * Destruct the slab descriptor on the cache of slab descriptors
  //    * Remove the slab descriptor from the correct slab list
}

//-------------------------------------------------------------
// Object Management
//-------------------------------------------------------------
}

#endif
