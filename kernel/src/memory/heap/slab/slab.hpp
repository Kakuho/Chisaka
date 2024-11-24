#ifndef MEMORY_HEAP_SLAB_HPP
#define MEMORY_HEAP_SLAB_HPP

#include <cstdint>
#include <new>

#include "cache_descriptor.hpp"
#include "slab_descriptor.hpp"
#include "./../../physical/freelist.hpp"

namespace Mem::Heap::Slab::T{

class SlabAllocator{ 

  template<typename T>
  struct CacheHandler{
    CacheDescriptor<T>* pcache = nullptr;
  };

  public:
    //-------------------------------------------------------------
    //  Initialisation
    //-------------------------------------------------------------
    explicit SlabAllocator();

    void Initialise();

  private:
    template<typename T>
    CacheDescriptor<T>* ConstructCache();

    template<typename T>
    void InitialiseKernelCache();

    void InitialiseGeneralCaches();

  public:
    //-------------------------------------------------------------
    // Heap Allocation Interface
    //-------------------------------------------------------------

    template<typename T>
    T* Allocate();

    template<typename T>
    void Free(CacheDescriptor<T>* pcache, T* pobject);

  private:
    Mem::Phys::FreeList& m_pageAllocator;

    // Kernel Objects
    CacheHandler m_cacheCache;
    CacheHandler m_slabCache;
    CacheHandler m_addressCache;
    CacheHandler<int> m_intCache;


    // General Heap Cache ... object sizes starts at 32 (2^5) and 
    // increases in powers of 2 up to (2^16)
    CacheHandler m_generalCache[11];
};

//-------------------------------------------------------------
// Specialised Cache Construction for Kernel Object Types
//-------------------------------------------------------------

template<>
CacheDescriptor* SlabAllocator::ConstructCache<CacheDescriptor>(){

}

//-------------------------------------------------------------
// Specialised Allocation for Kernel Object Types
//-------------------------------------------------------------

// Procedure for allocating within a slab:
//    i) first check of the Cache has a free slab
//    ii) if it does then allocate within it

template<>
inline CacheDescriptor* SlabAllocator::Allocate<CacheDescriptor>()
{
  if(m_cacheCache.pcache == nullptr){
    m_cacheCache.pcache = ConstructCache<CacheDescriptor>();
  }
  return nullptr;
}


template<>
inline SlabDescriptor* SlabAllocator::Allocate<SlabDescriptor>()
{
  if(m_slabCache.pcache == nullptr){

  }
  return nullptr;
}

// Template Specialisations for returning memory back to the caches 
// "free"

template<>
inline void SlabAllocator::Free<CacheDescriptor>(
    CacheDescriptor* pcache, 
    CacheDescriptor* pobject
  )
{
  // do something to cacheCaches
}

template<>
inline void SlabAllocator::Free<SlabDescriptor>(
    CacheDescriptor* pcache, 
    SlabDescriptor* pobject
  )
{
  // do something to cacheCaches
}

}

#endif
