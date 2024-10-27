#ifndef PAGES_HPP
#define PAGES_HPP

#include <cstdint>
#include <array>

#include "serial/kostream.hpp"
#include "memory/pmm/buddy.hpp"

extern "C" void set_paging_table(std::size_t address);

namespace memory{

struct PageFrameDescriptor{
  // bookkeeps details regarding page frames
  PageFrameDescriptor(void* baseaddress): 
    base{reinterpret_cast<paddr64_t>(baseaddress)}{

  }
  paddr64_t base;           // the base address of the pageframes
  /*std::size_t flags;*/    // to be determined later
  std::int32_t count;       // determines how many processes is referncing it.
  pmm::buddy::Node node;    // determines the next free block in the 
                            //    buddyallocator algorithm
};

class PageFrames{
  // class to represent all page frames as a sequential array
  public:
    PageFrames(const pmm::buddy::PhysicalMemory& pmmDescriptor);
    void Initialise(const pmm::buddy::PhysicalMemory& pmmDescriptor);
    void SetPageFrameBase(const pmm::buddy::PhysicalMemory& pmmDescriptor);
    void SetBuddyChunks(const pmm::buddy::PhysicalMemory& pmmDescriptor);

    std::uint64_t Size(const pmm::buddy::PhysicalMemory& pmmDescriptor ) const;
    void PrintBases() const;
    void PrintBases(std::uint64_t limit) const;

  private:
    PageFrameDescriptor* m_buffer; // a handler to the pageframes in memory
    std::uint64_t m_npf;
};

}

#endif
