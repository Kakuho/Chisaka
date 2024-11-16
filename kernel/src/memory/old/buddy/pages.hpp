#ifndef PAGES_HPP
#define PAGES_HPP

#include <cstdint>
#include <array>

#include "drivers/serial/kostream.hpp"
#include "x86_64/utilites.hpp"
#include "x86_64/features/features.hpp"
#include "memory/old/memory.hpp"
#include "buddy.hpp"
#include "memory/old/pstructs.hpp"
#include "limine/limine.h"
#include "limine/requests.hpp"

extern "C" void set_paging_table(std::size_t address);

namespace memory{

extern paddr64_t startOfRam;

inline std::size_t GetBootPages(){
  // function to return the number of pages available for use at boot
  using namespace limine;
  std::size_t bootpages = 0;
  std::uint64_t mem_entries_t = requests::memorymap_request.response->entry_count;
  limine_memmap_entry** entries = requests::memorymap_request.response->entries;
  for(std::uint64_t i = 0; i < mem_entries_t; i++){
    // only count memory mapping regions which are availabe
    if(entries[i]->type == LIMINE_MEMMAP_USABLE){
      std::uint64_t length = entries[i]->length;
      bootpages += length / 0x1000;
    }
  }
  return bootpages;
}

struct page{
  // page frame descriptor
  paddr64_t base;
  std::size_t flags;
  std::int32_t count;     // used to determine how many processes is referncing it.
  pmm::buddy::Node node;  // used to determine the next free block

};

struct PageFrameDescriptor{
  // bookkeeps details regarding page frames
  paddr64_t base;           // the base address of the pageframes
  /*std::size_t flags;*/    // to be determined later
  std::int32_t count;       // determines how many processes is referncing it.
  pmm::buddy::Node node;    // determines the next free block in the 
                            //    buddyallocator algorithm
};

extern void InitialisePageFrames();

inline void PrintSizeofPagetor(){
  kout << sizeof(page) << '\n';
}

extern void PrintSizeofPagetorMap();

extern void PrintAvailablePages();

extern void PrintPageFrames();

extern std::size_t GetAvailablePages();

extern void CheckBootPages();

extern paddr64_t PageAddress(std::size_t frameNumber);

inline std::size_t PhysicalToPageN(paddr64_t paddr){ 
  return (paddr - startOfRam) / 0x1000;
}

paddr64_t VirtualToPageBase(vaddr64_t vaddr);

//inline std::size_t VirtualToPageN(vaddr64_t vaddr);

}

#endif

