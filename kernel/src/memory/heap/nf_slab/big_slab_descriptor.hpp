#ifndef MEMORY_HEAP_NFSLAB_BIG_SLAB_DESCRIPTOR_HPP
#define MEMORY_HEAP_NFSLAB_BIG_SLAB_DESCRIPTOR_HPP

//  The slab descriptors has:
//    * A free list of object descriptors
//    * A doubly linked list to other slabs of the same type
//
//  Requirements:
//    * Slabs must initialise all of their objects before usage
//    * On deallocation, Slabs must Destruct their Object, and then 
//      deallocation can occur

#include <cstdint>
#include <new>

#include "lib/kassert.hpp"

#include "cache_descriptor.hpp"

namespace Mem::Heap::NFSlab::T{

template<typename T>
class CacheDescriptor;

template<typename T>
class BigSlabDescriptor{
  using FreeListHead = T*;
  friend class CacheDescriptor<T>;

  // Each page frame a slab refers to is divided into cells
  struct Cell{
    T object;
    Cell* pnext;
  };

  struct SlabFooter{
    CacheDescriptor<T>* cache;
    std::size_t allocated; 
    std::size_t totalObjects;; 
    FreeListHead nextFree;    // internal singly linked list
    BigSlabDescriptor<T>* next;  // doubly linked list of slabs
    BigSlabDescriptor<T>* prev; 
  };  // should be 44 bytes of memory + padding for alignment

  public:
    explicit BigSlabDescriptor(void* pageBase);
    ~BigSlabDescriptor();

    constexpr void* PageBase() const noexcept{
      return reinterpret_cast<void*>(
        reinterpret_cast<std::uint64_t>(m_footer) % 0x1000
      );
    }

    //-------------------------------------------------------------
    //  Object Management
    //-------------------------------------------------------------
  
    T* ObjectAddress(std::size_t index);

    template<typename... Args>
    T* Allocate(Args&&... args){
      T* allocd = new(m_footer->m_nextFree) T{std::forward<Args>(args)...};
      return allocd;
    }

    void Free(T* pobject);

    //-------------------------------------------------------------
    //  List Management
    //-------------------------------------------------------------

    BigSlabDescriptor* NextSlab() noexcept{return m_footer->next;}
    BigSlabDescriptor* PrevSlab() noexcept{return m_footer->prev;}

  private:
    // Helpers for lifetime
    void Initialise();
    void Reset();

  private:
    SlabFooter* m_footer;
};

//-------------------------------------------------------------
// Initialisation
//-------------------------------------------------------------

template<typename T>
BigSlabDescriptor<T>::BigSlabDescriptor(void* pageBase)
{
  Initialise();
}

// Given a page we need to initialise objects within the pages (Slab Allocator Specific)
// N.B Since i use placement new, operator new will construct the objects for me

template<typename T>
void BigSlabDescriptor<T>::Initialise(){
  std::size_t count = 1;
  std::uint64_t pageBase = reinterpret_cast<std::uint64_t>(PageBase());
  // Object Initialisation
  Cell* objectIndex = new(reinterpret_cast<Cell*>(pageBase)) Cell{};
  while( reinterpret_cast<std::uint64_t>(objectIndex+1) < 
        (reinterpret_cast<std::uint64_t>(pageBase) + 0x1000 - sizeof(SlabFooter))){
    Cell* objectNext = new(objectIndex+1) Cell{};
    objectIndex->pnext = objectNext;
    // update the index
    objectIndex = objectNext;
    count++;
  }
  // Slab Footer Allocation
  m_footer = reinterpret_cast<SlabFooter*>(
      reinterpret_cast<std::uint64_t>(objectIndex) + sizeof(Cell)
    );
  // populate known slab descriptor information
  m_footer->cache = nullptr;
  m_footer->allocated = 0;
  m_footer->totalObjects = count;
  m_footer->nextFree = reinterpret_cast<T*>(pageBase);
  m_footer->next = nullptr;
  m_footer->prev = nullptr;
}

template<typename T>
BigSlabDescriptor<T>::~BigSlabDescriptor(){
  Reset();
  // update the slab linked list
  if(PrevSlab()){
    PrevSlab()->m_footer->next = this->m_footer->next;
  }
  if(NextSlab()){
    if(!this->m_footer->prev){
      // update the cache's pointer to the next
    }
    NextSlab()->m_footer->prev = this->m_footer->prev;
  }
}

template<typename T>
void BigSlabDescriptor<T>::Reset(){
  // Essentially, calls the destructor for each object within the slab
  std::uint64_t pageBase = reinterpret_cast<std::uint64_t>(PageBase());
  Cell* index = reinterpret_cast<Cell*>(pageBase);
  index->~Cell();
  while(reinterpret_cast<std::uint64_t>(index+1) < 
      reinterpret_cast<std::uint64_t>(pageBase) + 0x1000){
    index++;
    index->~Cell();
  }
}

//-------------------------------------------------------------
//  Object Management
//-------------------------------------------------------------

template<typename T>
T* BigSlabDescriptor<T>::Allocate(){
  if(m_footer->allocated == m_footer->totalObjects){
    return nullptr;
  }
  Cell* alloc = reinterpret_cast<Cell*>(m_footer->nextFree);
  if(alloc->pnext != nullptr){
    m_footer->nextFree = reinterpret_cast<T*>(alloc->pnext);
    m_footer->allocated++;
    BigSlabDescriptor* head = m_footer->cache->PartialSlabsHead();
  }
  else{
    BigSlabDescriptor* head = m_footer->cache->FullSlabsHead();
  }
}

//-------------------------------------------------------------
// Misc
//-------------------------------------------------------------

void DoSomething(){
  BigSlabDescriptor<int> yes{reinterpret_cast<void*>(0x1000)};
  yes.Allocate(1);
}

}

#endif
