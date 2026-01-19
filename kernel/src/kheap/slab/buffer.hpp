#pragma once

#include <cstdint>
#include <new>

#include "list_descriptor.hpp"
#include "options.hpp"

// Buffers has unique ownership of their List of List Descriptors.
// When buffers deallocate, all of their list descriptors must also deallocate.

namespace Chisaka::Slab{

class Buffer{
  static constexpr std::uint8_t OP_LIMIT = 255; // used for gc
  public:
    Buffer();

    Buffer(std::uint16_t bufferSize);
    Buffer(std::uint16_t bufferSize, std::uint8_t options);
    ~Buffer();

    Buffer(const Buffer& src) = delete;
    Buffer(Buffer&& src) = delete;

    std::uint16_t BufferSize() const { return m_bufferSize;}
    void SetBufferSize(std::uint16_t sz){ m_bufferSize = sz;}

    void AddList(ListDescriptor* list);
    void TryAddFreeList(ListDescriptor* list);
    void TryAddPartialList(ListDescriptor* list);
    void TryAddFullList(ListDescriptor* list);

    // Move* is used to move list descriptors between lists
    void MoveFreeHead();
    void MovePartialHead();
    void MoveFullHead();

    void MoveFreeEntry(ListDescriptor* entry);
    void MovePartialEntry(ListDescriptor* entry);
    void MoveFullEntry(ListDescriptor* list);

    void* Allocate();
    void* AddToFree();
    void* AddToPartial();

    // quite slow deallocation routine
    using DeStatus = bool;
    void Deallocate(void* objaddr);
    DeStatus DeallocPartial(void* objaddr);   
    DeStatus DeallocFull(void* objaddr);

    void Grow();

    void RemoveListDescriptor(ListDescriptor* ld);
    void Reap();

    // REQUIRES CONTIGOUS
    void Grow(unsigned pages);
    void Reap(unsigned pages);

    ListDescriptor*& FreeListHead(){return m_freeLists;}
    ListDescriptor*& PartialListHead(){return m_partialLists;}
    ListDescriptor*& FullListHead(){return m_fullLists;}

    // Linked List Api - I need to refactor this because its duplicated from 
    // list descriptor
    Buffer* PrevBuffer() const {return m_prevBuffer;}
    Buffer*& PrevBuffer(){return m_prevBuffer;}
    Buffer* NextBuffer() const {return m_nextBuffer;}
    Buffer*& NextBuffer() {return m_nextBuffer;}

    void AddBuffer(Buffer* src);
    void Remove(Buffer* entry);
    Buffer* Extract(Buffer* entry);
    Buffer* ExtractSelf();

    // Iterator API
    Buffer* begin(){return this;}
    Buffer* end(){return this;}
    Buffer* next(){return m_nextBuffer;}

  private:
    ListDescriptor* m_freeLists;
    ListDescriptor* m_fullLists;
    ListDescriptor* m_partialLists;
    Buffer* m_nextBuffer;
    Buffer* m_prevBuffer;
    std::uint16_t m_bufferSize;
    std::uint8_t m_options;
    std::uint8_t m_opCount;
};

static_assert(sizeof(Buffer) <= 64);

}
