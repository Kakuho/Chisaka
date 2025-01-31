#ifndef MEMORY_HEAP_NFSLAB_SMALLSLAB_HPP
#define MEMORY_HEAP_NFSLAB_SMALLSLAB_HPP

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
class SmallSlab{
  using FreeListHead = T*;
  friend class CacheDescriptor<T>;

  static constexpr std::size_t PAGE_SIZE = 0x1000;

  // Each page frame a slab refers to is divided into cells
  struct Cell{
    T object;
    Cell* pnext;
  };

  struct SlabFooter{
    CacheDescriptor<T>* cache;
    std::size_t allocated; 
    std::size_t totalObjects; 
    FreeListHead nextFree;    // internal singly linked list
    SmallSlab<T>* next;  // doubly linked list of slabs
    SmallSlab<T>* prev; 
  };  // should be 44 bytes of memory + padding for alignment

  public:
    explicit SmallSlab();
    ~SmallSlab();

    //-------------------------------------------------------------
    //  Object Management
    //-------------------------------------------------------------
  
    T* ObjectAddress(std::size_t index);

    template<typename... Args>
    T* Allocate(Args&&... args);

    void Free(T* pobject);

    //-------------------------------------------------------------
    //  List Management API
    //-------------------------------------------------------------

    SmallSlab<T>*& NextSlab() noexcept{return m_footer.next;}
    SmallSlab<T>*& PrevSlab() noexcept{return m_footer.prev;}
    void Append(SmallSlab<T>* pslab) noexcept{
      SmallSlab<T>* oldTail = PrevSlab();
      PrevSlab() = pslab;
      pslab->NextSlab() = this;
      pslab->PrevSlab() = oldTail;
      oldTail->NextSlab() = pslab;
    }

    //-------------------------------------------------------------
    //  Others
    //-------------------------------------------------------------
    
    CacheDescriptor<T>*& Cache() noexcept{return m_footer.cache;}

    constexpr bool Full() noexcept
    { return m_footer.allocated == m_footer.totalObjects;}

    constexpr bool Empty() noexcept
    { return m_footer.allocated == 0;}

    constexpr const void* PageBase() const noexcept{
      kassert( reinterpret_cast<std::size_t>(this) % 0x1000 == 0);
      return this;
    }

  private:
    // Helpers for lifetime
    void Initialise();
    void Reset();

  private:
    char padding[PAGE_SIZE - sizeof(SlabFooter)];
    SlabFooter m_footer;
};

static_assert(sizeof(SmallSlab<int>) == 0x1000);
static_assert(sizeof(SmallSlab<std::uint64_t>) == 0x1000);
static_assert(sizeof(SmallSlab<void*>) == 0x1000);

//-------------------------------------------------------------
// Initialisation
//-------------------------------------------------------------

template<typename T>
SmallSlab<T>::SmallSlab()
{
  // Ensures the slab descriptor is allocated on a page boundary
  kassert(reinterpret_cast<std::size_t>(this) % 0x1000 == 0);
  Initialise();
}

// Given a page we need to initialise objects within the pages (Slab Allocator Specific)
// N.B Since i use placement new, operator new will construct the objects for me

template<typename T>
void SmallSlab<T>::Initialise(){
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
  // populate known slab descriptor information
  m_footer.cache = nullptr;
  m_footer.allocated = 0;
  m_footer.totalObjects = count;
  m_footer.nextFree = reinterpret_cast<T*>(pageBase);
  m_footer.next = nullptr;
  m_footer.prev = nullptr;
}

template<typename T>
SmallSlab<T>::~SmallSlab(){
  Reset();
}

template<typename T>
void SmallSlab<T>::Reset(){
  // Essentially, calls the destructor for each object within the slab
  std::uint64_t pageBase = reinterpret_cast<std::uint64_t>(PageBase());
  Cell* index = reinterpret_cast<Cell*>(pageBase);
  index->~Cell();
  while(reinterpret_cast<std::uint64_t>(index+1) < 
      reinterpret_cast<std::uint64_t>(pageBase) + 0x1000 - sizeof(SlabFooter)){
    index++;
    index->~Cell();
  }
}

//-------------------------------------------------------------
//  Object Management
//-------------------------------------------------------------

template<typename T> template<typename... Args>
T* SmallSlab<T>::Allocate(Args&&... args){
  T* allocd = new(m_footer.nextFree) T{std::forward<Args>(args)...};
  m_footer.nextFree = &(reinterpret_cast<Cell*>(m_footer.nextFree))->object;
  return allocd;
}

template<typename T>
void SmallSlab<T>::Free(T* pobject){
  // * Free the object
  // * Find the correct location to place the free'd object
  //    Walk the free list comparing the object location with the 
  // * May have to set the free object pointer
}

//-------------------------------------------------------------
// Misc
//-------------------------------------------------------------

void DoSomething(){
  SmallSlab<int> yes{};
  yes.Allocate(1);
}

}

#endif
