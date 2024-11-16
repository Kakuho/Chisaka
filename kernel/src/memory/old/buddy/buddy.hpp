#ifndef BUDDY_HPP
#define BUDDY_HPP

// the buddy contains linked lists to page descriptors.
// the idea is that each page frame descriptor contains information regarding 
// a particular page frame. In particular there are links to 
// other page frame descriptors, these links are precisely to start of the 
// next 2^order free chunks, where we use the standard ordering for our linear
// ordering of address.

#include <cstdint>

#include "limine/requests.hpp"
#include "memory/old/memory.hpp"
#include "drivers/serial/kostream.hpp"

namespace memory::pmm::buddy{

struct Node{
  void* next;
  void* previous;
};

class BuddyAllocator{
  public:
    BuddyAllocator();
    static constexpr std::uint8_t order = 10;

  private:
    void FindAvailableMemoryAreas();
    Node freeChunks[order+1]; // freeChunks is an array of doubly linked lists
                              // where element k contains links to 2^k free blocks
};

struct MemoryRange{
  paddr64_t lower;
  paddr64_t higher;
  // some helpful functions
  std::uint64_t Length() const { return higher - lower; }
  std::uint64_t Pages()  const { return Length() / 0x1000; }
  bool IsEmpty() const { return lower == higher; }
};

class FreeChunks{
  public:
    explicit FreeChunks(){
      InitialiseBuffer();
    }

    explicit FreeChunks(const MemoryRange& src): m_range{src}{
      InitialiseBuffer();
    }

    std::uint16_t& operator[](std::uint8_t index){ return buffer[index]; }
    const std::uint16_t& operator[](std::uint8_t index) const{ return buffer[index]; }

    std::uint64_t Base() const { return m_range.lower;}
    std::uint64_t Top() const { return m_range.higher;}

  private:
    std::uint16_t buffer[BuddyAllocator::order+1];
    MemoryRange m_range;
    void InitialiseBuffer(){ 
      for(auto& i: buffer) i = 0; 
    }
};

inline void PrintFreeChunk(const FreeChunks& chunkies){
  for(int i = BuddyAllocator::order; i >= 0; i--){
    kout << "i :: " << i << " :: chunkies[i] :: " << chunkies[i] << '\n';
  }
}

struct ChunkDescriptor{
  void* base;
  void* next;
};

// a class to bookkeep usuable physical memory at boot time
class PhysicalMemory{
  public:
    static constexpr std::uint8_t ranges = 6;
    explicit PhysicalMemory();

    // helper functions for the buddy system
    FreeChunks FindChunks(std::uint8_t index) const;
    void PrintChunkDescriptors(std::uint8_t rangeIdx) const;

    // other memory functions
    void PrintPhysicalMap() const;
    paddr64_t GetTopOfPhysicalRam() const;
    std::uint64_t GetAvailableMemory() const;
    std::uint64_t GetUsuablePages() const{ return GetAvailableMemory() / 0x1000; }

    const MemoryRange& GetRange(std::uint8_t index) const { return m_memories[index]; }

  private:
    MemoryRange m_memories[ranges];
    void PopulateRanges();
    bool IsEmptyRange(std::uint8_t index) const { 
      return (m_memories[index].lower == 0) && (m_memories[index].higher == 0);
    }
};

/*
inline void SetPageFrameDescriptors(void* pageframesdescriptors){
  // procedure to populate the page frame descriptors with base values 
  // and linked list nodes
}
*/

} // namespace memory::pmm

#endif
