#ifndef MEMORY_HEAP_NFSLAB_CACHE_DESCRIPTOR_HPP
#define MEMORY_HEAP_NFSLAB_CACHE_DESCRIPTOR_HPP

//  Cache Descriptors and related functionalities
//
//  Each Cache Descriptor has the head of a doubly linked list of its slab 
//  descriptors

#include <cassert>
#include <utility>

#include "small_slab.hpp"
#include "big_slab_descriptor.hpp"
#include "slab_list.hpp"
#include "memory/physical/bitmap.hpp"
#include "./../../physical/freelist.hpp"

namespace Mem::Heap::NFSlab::T{

template<typename T>
class SlabDescriptor;

template<typename T>
class CacheDescriptor{
  static constexpr std::size_t SMALL_LIMIT = 100;
  using PageAllocator_t = Mem::Phys::Bitmap;
  
  public:
    using SlabType_t = std::conditional<sizeof(T) < SMALL_LIMIT, 
          SmallSlab<T>, BigSlabDescriptor<T>>::type;

  public:
    //-------------------------------------------------------------
    //  Lifetime
    //-------------------------------------------------------------

    explicit CacheDescriptor();

    //-------------------------------------------------------------
    // Slab Management
    //-------------------------------------------------------------

    SlabType_t* 
    AllocateSlab(PageAllocator_t& pageAllocator) noexcept;

    void FreeAppendAdd(SlabType_t* pslab) noexcept;
    void PartialAppendAdd(SlabType_t* pslab) noexcept;
    void FullAppendAdd(SlabType_t* pslab) noexcept;

    void FreeSlab(SlabType_t* pslab);
    void DestroySlabs();  // iteratively delete all slabs
                          
    //-------------------------------------------------------------
    // Object Management
    //-------------------------------------------------------------

    template<typename ...Args>
    T* AllocateObject(Args&&... args);

    void FreeObject(T* pobject);

  private:
    SlabType_t* m_freeSlabs;
    SlabType_t* m_partialSlabs;
    SlabType_t* m_fullSlabs;
    // used for quick querying
    std::size_t objectsPerSlab;
    std::size_t freeObjects;    
};

//-------------------------------------------------------------
//  Lifetime
//-------------------------------------------------------------

template<typename T>
CacheDescriptor<T>::CacheDescriptor():
  m_freeSlabs{nullptr},
  m_partialSlabs{nullptr},
  m_fullSlabs{nullptr}
{
}

//-------------------------------------------------------------
// Slab Management 
//-------------------------------------------------------------

template<typename T>
CacheDescriptor<T>::SlabType_t* 
CacheDescriptor<T>::AllocateSlab(PageAllocator_t& pageAllocator)
noexcept{
  // Only touches the Free Slab List
  //void* base = pageAllocator.AllocatePage();
  void* base = reinterpret_cast<void*>(0xFF000);
  SlabType_t* allocSlab = new(base) SlabType_t{};
  allocSlab->Cache() = this;
  FreeAppendAdd(allocSlab);
  return allocSlab;
}

// Design Consideration:
//    * A Slab List is a doubly linked list
//    * When a Slab List has a singular element, the element sibilings is itself, 
//      and thus never empty

template<typename T>
void CacheDescriptor<T>::FreeAppendAdd(SlabType_t* pslab) noexcept{
  if(m_freeSlabs){
    m_freeSlabs->Append(pslab);
  }
  else{
    m_freeSlabs = pslab;
    m_freeSlabs->NextSlab() = m_freeSlabs;
    m_freeSlabs->PrevSlab() = m_freeSlabs;
  }
}

template<typename T>
void CacheDescriptor<T>::PartialAppendAdd(SlabType_t* pslab) noexcept{
  if(m_partialSlabs){
    m_partialSlabs->Append(pslab);
  }
  else{
    m_partialSlabs = pslab;
    m_partialSlabs->NextSlab() = m_partialSlabs;
    m_partialSlabs->PrevSlab() = m_partialSlabs;
  }
}

template<typename T>
void CacheDescriptor<T>::FullAppendAdd(SlabType_t* pslab) noexcept{
  if(m_fullSlabs){
    m_fullSlabs->Append(pslab);
  }
  else{
    m_fullSlabs = pslab;
    m_fullSlabs->NextSlab() = m_fullSlabs;
    m_fullSlabs->PrevSlab() = m_fullSlabs;
  }
}

template<typename T>
void CacheDescriptor<T>::FreeSlab(SlabType_t* pslab){
  // Common Clean Up Procedure :
  if((pslab->NextSlab() != nullptr) && (pslab->PrevSlab() != nullptr)){
    pslab->NextSlab()->PrevSlab() = pslab->PrevSlab();
    pslab->PrevSlab()->NextSlab() = pslab->NextSlab();
  }
  //    * Release the page frame back to the page frame allocator
  void* pagebase = pslab->PageBase();
  //    * Call Destructor on the objects of the Slab
  delete pslab;
}

//-------------------------------------------------------------
// Object Management
//-------------------------------------------------------------

template<typename T> template<typename ...Args>
T* CacheDescriptor<T>::AllocateObject(Args&&... args){
  // we want to allocate from the partial slabs first for space efficency
  if(m_partialSlabs){
    m_partialSlabs->Allocate(args...);
    if(m_partialSlabs->Full()){
      if(m_partialSlabs->NextSlab() == m_partialSlabs){
        FullAppendAdd(m_partialSlabs);
        m_partialSlabs = nullptr;
      }
      else{
        SlabType_t*& newPartialHead = m_partialSlabs->NextSlab();
        m_partialSlabs->PrevSlab()->NextSlab() = newPartialHead;
        newPartialHead->PrevSlab() = m_partialSlabs->PrevSlab();
        FullAppendAdd(m_partialSlabs);
        m_partialSlabs = newPartialHead;
      }
    }
  }
  else if(m_freeSlabs){
    if(m_freeSlabs->NextSlab() == m_freeSlabs){
      m_freeSlabs->Allocate(args...);
      PartialAppendAdd(m_freeSlabs);
      m_freeSlabs = nullptr;
    }
    else{
      SlabType_t*& newFreeHead = m_freeSlabs->NextSlab();
      m_freeSlabs->PrevSlab()->NextSlab() = newFreeHead;
      newFreeHead->PrevSlab() = m_freeSlabs->PrevSlab();
      m_freeSlabs->Allocate(args...);
      PartialAppendAdd(m_freeSlabs);
      m_freeSlabs = newFreeHead;
    }
  }
  else{
    return nullptr;
  }
}
    
template<typename T>
void CacheDescriptor<T>::FreeObject(T* pobject){

}

//-------------------------------------------------------------
//  Misc
//-------------------------------------------------------------

void CacheTemplateChecker(){
  Mem::Phys::Bitmap pageAllocator{};
  CacheDescriptor<int> cache{};
  cache.AllocateSlab(pageAllocator);
}

}

#endif
