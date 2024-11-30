#ifndef MEMORY_HEAP_SLAB_CACHE_DESCRIPTOR_HPP
#define MEMORY_HEAP_SLAB_CACHE_DESCRIPTOR_HPP

//  Cache Descriptors and related functionalities
//
//
//  Each Cache Desriptor has the head of a doubly linked list of its slab 
//  descriptors

#include <cassert>

#include "slab_descriptor.hpp"
#include "slab_list.hpp"
#include "./../../physical/freelist.hpp"

namespace Mem::Heap::Slab::T{

template<typename T>
class SlabDescriptor;

template<typename T>
class CacheDescriptor{
  using PageAllocator_t = Mem::Phys::FreeList;
  public:
    //-------------------------------------------------------------
    //  Lifetime
    //-------------------------------------------------------------

    explicit CacheDescriptor();

    //-------------------------------------------------------------
    // Slab Management
    //-------------------------------------------------------------

    [[nodiscard]] SlabDescriptor<T>* 
    AllocateSlab(PageAllocator_t& pageAllocator) noexcept;
    void FreeSlab(SlabDescriptor<T>* pslab);
    void DestroySlabs();  // iteratively delete all slabs

    SlabDescriptor<T>* PartialSlabsHead(){ return partialSlabs;}
    SlabDescriptor<T>* FullSlabsHead(){ return fullSlabs;}
    SlabDescriptor<T>* EmptySlabsHead(){ return partialSlabs;}
    
    //-------------------------------------------------------------
    // Object Management
    //-------------------------------------------------------------

    void Alloc();
    void Free(T* pobject);

  private:
    SlabList<T> partialSlabs;
    SlabList<T> fullSlabs;
    SlabList<T> freeSlabs;
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
CacheDescriptor<T>::AllocateSlab(PageAllocator_t& pageAllocator) 
noexcept{
  //  Procedure:
  //    * obtain a page frame
  //void* base = m_pmmAllocator.AllocatePage();
  //    * allocate a slab descriptor on the cache of slab descriptors 
  //    * initialise the slab
  //    * Initialise the objects of the slab
  //    * add the slab to the list of free slabs 
  void* base = pageAllocator.AllocatePage();
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
