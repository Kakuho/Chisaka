/*
#ifndef MEMORY_HEAP_SLAB_DESCRIPTORS_HPP
#define MEMORY_HEAP_SLAB_DESCRIPTORS_HPP


// Descriptors for the Slab Allocator to facilliate usage

#include <cstdint>

namespace Mem::Heap::Slab{

// information regarding a slab
// A slab requires:
//    A free list of object part of its page
//    A doubly linked list to other slabs part of its cache
//    A Colouring Area
struct SlabDescriptor{
  void* nextFreeObject;   // linked lists of object 
  void* pageBase;
  void* parentCache;
  // doubly linked list of slabs
  SlabDescriptor* next;
  SlabDescriptor* prev;
  std::size_t allocated;

  //-------------------------------------------------------------
  //  Functionalities
  //-------------------------------------------------------------

  SlabDescriptor(void* pageBase);
};


enum class CacheType: std::uint8_t{
  CacheDescriptor,
  SlabDescriptor,
  AddressSpaceDescriptor
};

struct CacheDescriptor{
  CacheType type;
  CacheDescriptor* nextCache;   // caches are organised as a linked list
  // lists of slabs
  SlabDescriptor* partialSlabs;
  SlabDescriptor* fullSlabs;
  SlabDescriptor* freeSlabs;
  // used for quick querying
  std::size_t objectSize;
  std::size_t objectsPerSlab;
  std::size_t freeObjects;    

  //-------------------------------------------------------------
  // Functionalities
  //-------------------------------------------------------------

  CacheDescriptor(CacheType type)
    : type{type},
      nextCache{nullptr},
      partialSlabs{nullptr},
      fullSlabs{nullptr},
      freeSlabs{nullptr}
  {

  }
  
  // Increasing a Slab requires:
  //  i)    Page Frames to be allocated
  //  ii)   Initialising the slab
  //  iii)  Initialisaing the objects of that slab
  //  iv)   adding the slab descriptor to the list of free slabs
  SlabDescriptor* IncreaseSlab(std::size_t n); 
  void InitialiseObjects(SlabDescriptor* pslab);

  // Releasing a slab requires:
  //  i)    calling the destructor on all slabs
  //  ii)   releasing the page frame back to the page frame 
  //        allocator
  //  iii)  
  void DeallocateSlab(SlabDescriptor* pslab);
  void DestroySlabs();  // destroys all the slabs of this cache
  
  // Allocate/Deallocate within a Cache
  void Alloc();
  void Free(void* pobject);

};


}

#endif
