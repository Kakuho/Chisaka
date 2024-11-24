#ifndef MEMORY_HEAP_SLAB_CACHE_DESCRIPTOR_HPP
#define MEMORY_HEAP_SLAB_CACHE_DESCRIPTOR_HPP

// Cache Descriptors and related functionalities

#include <cassert>

#include "slab_descriptor.hpp"

namespace Mem::Heap::Slab::T{

template<typename T>
class CacheDescriptor{
  public:
    //-------------------------------------------------------------
    //  Lifetime
    //-------------------------------------------------------------

    explicit CacheDescriptor();

    //-------------------------------------------------------------
    // Slab Management
    //-------------------------------------------------------------

    SlabDescriptor<T>* AllocateSlab();
    void FreeSlab(SlabDescriptor<T>* pslab);
    void DestroySlabs();  // iteratively delete all slabs
    
    //-------------------------------------------------------------
    // Object Management
    //-------------------------------------------------------------

    void Alloc();
    void Free(T* pobject);

  private:
    // lists of slabs
    SlabDescriptor<T>* partialSlabs;
    SlabDescriptor<T>* fullSlabs;
    SlabDescriptor<T>* freeSlabs;
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
SlabDescriptor<T>* CacheDescriptor<T>::AllocateSlab(){
  //  Procedure:
  //    * obtain a page frame
  //    * allocate a slab descriptor on the cache of slab descriptors 
  //    * initialise the slab
  //    * Initialise the objects of the slab
  //    * add the slab to the list of free slabs
}

template<typename T>
void CacheDescriptor<T>::FreeSlab(SlabDescriptor<T>* pslab){
  //  Procedure:
  //    * Call Destructor on the objects of the Slab
  //    * Release the page frame back to the page frame allocator
  //    * Destruct the slab descriptor on the cache of slab descriptors
  //    * Remove the slab descriptor from the correct slab list
}

}

#endif
