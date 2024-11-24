#ifndef MEMORY_HEAP_SLAB_SLAB_DESCRIPTOR_HPP
#define MEMORY_HEAP_SLAB_SLAB_DESCRIPTOR_HPP

//  The slab descriptors has:
//    * A free list of object descriptors
//    * A doubly linked list to other slabs of the same type
//
//  This class is specifcally for small kernel objects
//
//  Requirements:
//    
//    * Slabs must initialise all of their objects before usage
//    * On deallocation, Slabs must Destruct their Object, and then deallocation can occur

#include <cstdint>

namespace Mem::Heap::Slab::T{

template<typename T>
class SlabDescriptor{
  using FreeListHead = T*;

  // Each page frame a slab refers to is divided into cells
  struct Cell{
    T object;
    T* pnext;
  };

  // this is stored at the end of a large slab
  struct SlabInfo{
    struct kmembuf_ctl{
      T* pobject;
      T* next;
      bool free;
    };
    kmembuf_ctl* objectDesc;
    std::size_t nObjects;
  };

  public:
    explicit SlabDescriptor(void* pageBase);
    ~SlabDescriptor();

    T* ObjectAddress(std::size_t index);
  
  private:
    void InitialiseSmall();
    void InitialiseLarge();
    void ResetSmall();
    void ResetLarge();

  private:
    void* m_pageBase;
    void* m_cache;
    SlabInfo* m_slabInfo;
    std::size_t m_allocated;
    std::size_t m_totalObjects;
    // lists
    FreeListHead m_nextFree;
    // doubly linked list of slabs
    SlabDescriptor<T>* m_next;
    SlabDescriptor<T>* m_prev;
};

//-------------------------------------------------------------
// Initialisation
//-------------------------------------------------------------

template<typename T>
SlabDescriptor<T>::SlabDescriptor(void* pageBase)
  : m_pageBase{pageBase},
    m_cache{nullptr},
    m_allocated{0},
    m_totalObjects{0},
    m_nextFree{nullptr},
    m_next{nullptr},
    m_prev{nullptr}
{
  if(sizeof(T) < 0x1000){
    InitialiseSmall();
  }
  else{
    InitialiseLarge();
  }

}

// Given a page we need to initialise objects within the pages
// N.B Since i use placement new, operator new will construct the objects for me

template<typename T>
void SlabDescriptor<T>::InitialiseSmall(){
  // This procedure is called during construction when the type to store is a small type
  std::size_t count = 1;
  Cell* objectIndex = new(reinterpret_cast<Cell*>(m_pageBase)) Cell{};
  while( reinterpret_cast<std::uint64_t>(objectIndex) < reinterpret_cast<std::uint64_t>(m_pageBase) + 0x1000){
    Cell* objectNext = new(objectIndex+1) Cell{};
    objectIndex->pnext = objectNext;
    objectIndex++;
    count++;
  }
  // populate known slab descriptor information
  m_totalObjects = count;
  m_nextFree = m_pageBase;
}

template<typename T>
void SlabDescriptor<T>::InitialiseLarge(){
  //  Large is defined to be >= 0x1000
  //  Main difference is that the SlabInfo has to be alloacted within the slab
}

//-------------------------------------------------------------
//  Destruction
//-------------------------------------------------------------

template<typename T>
SlabDescriptor<T>::~SlabDescriptor(){
  // first call the destructors
  if(sizeof(T) < 0x1000){
    ResetSmall();
  }
  else{
    ResetLarge();
  }
  // clean the slab list
  m_next->m_prev = m_prev;
  m_prev->m_next = m_next;
}

template<typename T>
void SlabDescriptor<T>::ResetSmall(){
  Cell* index = m_pageBase;
  index.~Cell();
  while(reinterpret_cast<std::uint64_t>(index) < 
      reinterpret_cast<std::uint64_t>(m_pageBase) + 0x1000){
    index++;
    index.~Cell();
  }
}

}

#endif
