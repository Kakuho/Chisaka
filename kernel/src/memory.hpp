#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <limine.h>
#include <limine_services.hpp>
#include <kostream.hpp>

namespace req{
extern volatile limine_hhdm_request hhdm_request;
extern volatile limine_memmap_request memorymap_request;
}

namespace mem{
  using paddr64_t = std::uint64_t;
  using vaddr64_t = std::uint64_t;

  inline paddr64_t vaddrToPaddr(vaddr64_t vaddr){
    return vaddr - req::hhdm_request.response->offset;
  }

  inline vaddr64_t paddrToVaddr(paddr64_t paddr){
    return paddr + req::hhdm_request.response->offset;
  }

  void printMemoryMap(){
    // prints in the format:
    //
    //  paddr \t vaddr \t length \t type
    if(req::memorymap_request.response == nullptr){
      kout << "MEM FAIL\n";
    }
    std::uint64_t hhdm_offset = req::hhdm_request.response->offset;
    std::uint64_t mem_entries_t = req::memorymap_request.response->entry_count;
    limine_memmap_entry** entries = req::memorymap_request.response->entries;
    for(auto i = 0; i < mem_entries_t; i++){
      // now set the responses
      paddr64_t base = entries[i]->base;
      std::uint64_t length = entries[i]->length;
      std::uint64_t type = entries[i]->type;
      kout << intmode::hex << base << '\t' << paddrToVaddr(base) << '\t'
           << length << '\t' << type << '\n';
    }
  }

  void probeMemory(std::size_t index){
    // probe a single entry, entries[index]
    if(req::memorymap_request.response->entries[index] == nullptr){
      kout << "PROBING FAILED: " << index << '\n';
      return;
    }
    std::uint64_t hhdm_offset = req::hhdm_request.response->offset;
    limine_memmap_entry** entries = req::memorymap_request.response->entries;
    std::uint64_t type = entries[index]->type;
    if(type != LIMINE_MEMMAP_USABLE){
      kout << "THE MEMORY BLOCK ENTRY IS UNUSABLE: " << index << '\n';
      return;
    }
    paddr64_t base = entries[index]->base;
    std::uint64_t length = entries[index]->length;
    kout << "starting to probe memory" << '\n';
    for(paddr64_t paddr = base; paddr < base + length; paddr++){
      char* prober = reinterpret_cast<char*>(paddrToVaddr(paddr));
      *prober = 'A';
      kout << intmode::hex 
           << "base = " << base
           << " length = " << length
           << ", paddr = " << paddr 
           << ", vaddr = " << paddrToVaddr(paddr) 
           << ", value = " << *prober << '\n';
    }
    kout << "DONE!";
  }
} // namespace mem

#endif
