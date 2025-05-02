#pragma once

// Class abstraction for Partitions whose metadata lives on the page.
// Note Partitions has generic slots for buffers

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <utility>

#include "kassert.hpp"
#include "drivers/serial/kostream.hpp"
#include "aii/array.hpp"

namespace Mem::Heap{

class ListDescriptor{
  static constexpr std::uint16_t PAGE_SIZE = 0x1000;

  public:
    ListDescriptor() = delete;
    ListDescriptor(void* baseAddr, std::uint16_t bufferSize, std::uint8_t pages);
    ListDescriptor(ListDescriptor&& src);
    ListDescriptor& operator=(ListDescriptor&& src);
    ~ListDescriptor();

    void PoisonFreeArea(std::uint8_t val);
    void SetupLinkage();
    void* NextFreeBuffer() const { return m_nextFree;}

    constexpr std::uint16_t BufferSize() const { return m_bufferSize;}
    constexpr std::uint8_t Pages()      const {return m_pages;}
    std::uint16_t TotalBuffers()  const { return m_totalBuffers;}
    std::uint16_t BuffersUsed()  const { return m_buffersUsed;}

    std::uintptr_t Base() const
    {return reinterpret_cast<std::uintptr_t>(m_baseAddress);}
    std::uintptr_t Limit() const;
    std::uint16_t InitialFreeSpace()  const;
    std::uint16_t SpaceAfterAllocation() const;

    bool Full() const {return m_totalBuffers == m_buffersUsed;}
    bool Empty() const {return m_buffersUsed == 0;}
    bool HasSpace() const {return m_totalBuffers > m_buffersUsed;}

    void* Allocate();
    void Deallocate(void* address);

    bool ContainsObject(void* obj) const{ return ObjectInRange(obj);}
    bool ObjectInRange(void* obj) const;
    std::uint8_t BufferIndex(void* pbuffer) {
      return (reinterpret_cast<std::uintptr_t>(pbuffer) - Base()) / BufferSize();
    } // only problem is we kinda want to determine the index independent of
      // the slab :/

    // Linked List API
    ListDescriptor* PrevList() const {return m_prevList;}                   
    ListDescriptor*& PrevList(){return m_prevList;}                   
    ListDescriptor* NextList() const {return m_nextList;}                     
    ListDescriptor*& NextList() {return m_nextList;}                     

    void AddList(ListDescriptor* src); 
    void Remove(ListDescriptor* entry);
    ListDescriptor* Extract(ListDescriptor* entry);
    ListDescriptor* ExtractSelf();

    // iterator support
    ListDescriptor* begin() {return this;}
    ListDescriptor* end() {return this;}
    ListDescriptor* next() {return m_nextList;}

  private:
    void* m_baseAddress;
    void* m_nextFree; 
    //std::uintptr_r* m_nextFree;
    ListDescriptor* m_nextList;
    ListDescriptor* m_prevList;
    std::uint16_t m_bufferSize;
    std::uint16_t m_totalBuffers;
    std::uint16_t m_buffersUsed;
    std::uint8_t m_pages;
};

static_assert(sizeof(ListDescriptor) <= 64);

}
