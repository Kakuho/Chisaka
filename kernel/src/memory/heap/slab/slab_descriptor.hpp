#ifndef MEMORY_HEAP_SLAB_SLAB_DESCRIPTOR_HPP
#define MEMORY_HEAP_SLAB_SLAB_DESCRIPTOR_HPP

#include <cstdint>

namespace Mem::Heap::Slab::T{

template<typename T>
class ObjectDescriptor{
  // A Object Descriptor that's free, points to other
  // free object descriptors
  public:
    explicit ObjectDescriptor(T* objectBase)
      : m_next{nullptr},
        m_pobject{objectBase},
        m_free{true}
    {

    }

  private:
    ObjectDescriptor<T>* m_next;
    T* m_pobject;
    bool m_free;
};

static_assert(sizeof(ObjectDescriptor<void>) == 24);

//-------------------------------------------------------------

// The slab descriptors has:
//  * A free list of object descriptors
//  * A doubly linked list to other slabs of the same type
template<typename T>
class SlabDescriptor{
  using FreeListHead = ObjectDescriptor<T>*;
  public:
    explicit SlabDescriptor(void* pageBase);
  private:
    void InitialiseObjects();
    void InitialiseAsFreeObjectSlab();
    void InitialiseAsCacheDescriptorSlab();
    void InitialiseAsSlabDescriptorSlab();

  private:
    void* pageBase;
    void* parentCache;
    FreeListHead m_objectList;
    // doubly linked list of slabs
    SlabDescriptor<T>* next;
    SlabDescriptor<T>* prev;
    std::size_t allocated;
};

}

#endif
