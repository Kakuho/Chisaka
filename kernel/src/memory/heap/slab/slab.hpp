#ifndef MEMORY_HEAP_SLAB_HPP
#define MEMORY_HEAP_SLAB_HPP

#include <cstdint>
#include <new>

#include "./../../physical/freelist.hpp"
#include "descriptors.hpp"

namespace Mem::Heap::Slab{

class SlabAllocator{ 

  struct CacheHandler{
    CacheDescriptor* pcache = nullptr;
  };

  public:
    //-------------------------------------------------------------
    //  Initialisation
    //-------------------------------------------------------------
    explicit SlabAllocator();

    void Initialise(){
      // first create the cache of caches
      void* cachePage = m_pageAllocator.AllocatePage();
      CacheDescriptor* cacheCache = new(cachePage) 
        CacheDescriptor{CacheType::CacheDescriptor};
      // a cache of slabs is also required 
      CacheDescriptor* slabCache = new(cachePage) 
        CacheDescriptor{CacheType::SlabDescriptor};
      // now create initial slabs for that cache
      void* slabPage = m_pageAllocator.AllocatePage();
      SlabDescriptor* slab = reinterpret_cast<SlabDescriptor*>(cachePage);

      m_cacheCache.pcache = reinterpret_cast<CacheDescriptor*>
        (m_pageAllocator.AllocatePage());
      m_slabCache.pcache = reinterpret_cast<CacheDescriptor*>
        (m_pageAllocator.AllocatePage());
    }

  private:
    template<typename T>
    CacheDescriptor* ConstructCache();

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
    void Free(CacheDescriptor* pcache, T* pobject);

  private:
    Mem::Phys::FreeList& m_pageAllocator;

    // Kernel Objects
    CacheHandler m_cacheCache;
    CacheHandler m_slabCache;
    CacheHandler m_addressCache;

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
