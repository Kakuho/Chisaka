#pragma once

#include <cstdint>

#include "list_descriptor.hpp"

// Buffers has unique ownership of their List of List Descriptors.
// When buffers deallocate, all of their list descriptors must also be deallocated

namespace Chisaka::Slab::v2{

class Buffer{
  static constexpr std::uint8_t OP_LIMIT = 255; // used for gc
                                                //
  public:
    Buffer();
    Buffer(std::uint16_t bufferSize);
    ~Buffer();

    std::uint16_t BufferSize() const { return m_bufferSize;}
    bool Empty();
    bool Full();

    ListDescriptor*& ListHead(){return m_lists;}
    void AddList(ListDescriptor* list);
    bool OwnsList(ListDescriptor* list);
    void RemoveList(ListDescriptor* ld);
    void ReapLists();

    bool OwnsObject(void* pobj);

    void* Allocate();                         // O(n)
    void Deallocate(void* objaddr);           // O(n)

  private:
    ListDescriptor* m_lists;
    std::uint16_t m_bufferSize;
    std::uint8_t m_options;
    std::uint8_t m_opCount;                   // for reaping later
};

}
