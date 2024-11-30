#ifndef MEMORY_HEAP_SLAB_SLAB_LIST_HPP
#define MEMORY_HEAP_SLAB_SLAB_LIST_HPP

// Class to represent the head of doubly linked list of Slab Descriptors
// used in Cache Descriptors

#include "slab_descriptor.hpp"

namespace Mem::Heap::Slab::T{

template<typename T>
class SlabDescriptor;

template<typename T>
class SlabList{
  public:
    SlabList(SlabDescriptor<T>* head) noexcept: m_head{head}{}

    SlabDescriptor<T>* Head() noexcept { return m_head;}
    void AddBack(SlabDescriptor<T>& src);
    void AddBack(SlabDescriptor<T>&& src);
    void AddFront(SlabDescriptor<T>& src);
    void AddFront(SlabDescriptor<T>&& src);
    void Remove(std::size_t index);

  private:
    SlabDescriptor<T>* m_head;
};

template<typename T>
void SlabList<T>::AddBack(SlabDescriptor<T>& src){

}

template<typename T>
void SlabList<T>::AddBack(SlabDescriptor<T>&& src){

}

}

#endif
