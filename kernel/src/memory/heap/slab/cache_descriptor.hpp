#ifndef MEMORY_HEAP_SLAB_CACHE_DESCRIPTOR_HPP
#define MEMORY_HEAP_SLAB_CACHE_DESCRIPTOR_HPP

// Cache Descriptors and related functionalities

#include "slab_descriptor.hpp"
#include <cassert>

namespace Mem::Heap::Slab::T{

template<typename T>
class CacheDescriptor{
  public:
    CacheDescriptor();

    //-------------------------------------------------------------
    // Functionalities
    //-------------------------------------------------------------

    // Increasing a Slab requires:
    //  i)    Page Frames to be allocated
    //  ii)   Initialising the slab
    //  iii)  Initialisaing the objects of that slab
    //  iv)   adding the slab descriptor to the list of free slabs
    SlabDescriptor<T>* IncreaseSlab(std::size_t n); 

    // Releasing a slab requires:
    //  i)    calling the destructor on all slabs
    //  ii)   releasing the page frame back to the page frame 
    //        allocator
    void DeallocateSlab(SlabDescriptor<T>* pslab);

    // iteratively deleting all slabs of the 
    void DestroySlabs();
    
    // Allocate/Deallocate within a Cache
    void Alloc();
    void Free(void* pobject);

  private:
    // lists of slabs
    SlabDescriptor<T>* partialSlabs;
    SlabDescriptor<T>* fullSlabs;
    SlabDescriptor<T>* freeSlabs;
    // used for quick querying
    std::size_t objectSize;
    std::size_t objectsPerSlab;
    std::size_t freeObjects;    
};

}

#endif
