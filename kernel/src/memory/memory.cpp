#include "memory.hpp"

namespace mem{
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
      kout << "THE MEMORY BLOCK ENTRY IS UNUSABLE::TYPE::" << type << "::INDEX::" << index << '\n';
      return;
    }
    paddr64_t base = entries[index]->base;
    std::uint64_t length = entries[index]->length;
    kout << "starting to probe memory" << '\n'  
         << "max offset should be: " << intmode::hex << base + length << '\n';
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

  std::uint64_t calculateAvailableMemory(){
    if(req::memorymap_request.response == nullptr){
      kout << "MEM FAIL\n";
    }
    std::uint64_t available_memory;
    std::uint64_t mem_entries_t = req::memorymap_request.response->entry_count;
    limine_memmap_entry** entries = req::memorymap_request.response->entries;
    for(auto i = 0; i < mem_entries_t; i++){
      if(entries[i]->type == LIMINE_MEMMAP_USABLE){
        // only count memory mapping regions which are availabe
        std::uint64_t length = entries[i]->length;
        available_memory += length;
      }
    }
    return available_memory;
  }

  void printPageFrames(){

    std::uint64_t mem_entries_t = req::memorymap_request.response->entry_count;
    limine_memmap_entry** entries = req::memorymap_request.response->entries;
    for(auto i = 0; i < mem_entries_t; i++){
      // only count memory mapping regions which are availabe
      if(entries[i]->type == LIMINE_MEMMAP_USABLE){
        std::uint64_t base = entries[i]->base;
        std::uint64_t length = entries[i]->length;
        for(paddr64_t paddr = base; paddr < base + length; paddr += 0x1000){
          // n.b 0x1000 = 4096 = page size
          kout << "page: " << paddr << " to " << paddr + 0x1000 << '\n';
        }
      }
    }
  }

  vaddr64_t getKernelVirtualAddress(){
    return req::kernel_addr_req.response->virtual_base;
  }

  void upperLimitProbe(){
    // procedure to probe the upper limit of usable ram
    std::uint64_t mem_entries_t = req::memorymap_request.response->entry_count;
    limine_memmap_entry* entry = req::memorymap_request.response->entries[mem_entries_t - 1];
    std::uint64_t length = entry->length;
    mem::paddr64_t base = entry->base;
    kout << intmode::dec << "base: " << base << ", length = " << length << '\n';
    // now we do massive probe
    for(mem::paddr64_t paddr = base + length - 50;; paddr++){
      char* pch = reinterpret_cast<char*>(mem::paddrToVaddr(paddr));
      *pch = 'C';
      kout << "paddr = " << paddr << " :: vaddr = " << mem::paddrToVaddr(paddr)
           << " :: *pch = " << *pch << '\n';
    }
  }

  void lowerLimitProbe(){
    // procedure to probe the lower limit of usable ram
    std::uint64_t mem_entries_t = req::memorymap_request.response->entry_count;
    limine_memmap_entry* entry = req::memorymap_request.response->entries[0];
    std::uint64_t length = entry->length;
    mem::paddr64_t base = entry->base;
    kout << intmode::hex << "base: " << base << ", length = " << length << '\n';
    // now we do massive probe
    for(mem::paddr64_t paddr = base + 50;; paddr--){
      char* pch = reinterpret_cast<char*>(mem::paddrToVaddr(paddr));
      *pch = 'C';
      kout << "paddr = " << paddr << " :: vaddr = " << mem::paddrToVaddr(paddr)
           << " :: *pch = " << *pch << '\n';
    }
  }

  /* DO NOT USE*/
  void corruptMemory(){
    // procedure to completely corrupt the ram >:)
    std::uint64_t mem_entries_t = req::memorymap_request.response->entry_count;
    limine_memmap_entry* entry_final = req::memorymap_request.response->entries[mem_entries_t -1];
    mem::paddr64_t base_final = entry_final->base;
    mem::paddr64_t length_final = entry_final->length;
    //kout << intmode::hex << "base: " << base << ", length = " << length << '\n';
    // now we do massive probe - we start from zero to the maximum
    for(mem::paddr64_t paddr = 0; paddr < base_final + length_final; paddr++){
      char* pch = reinterpret_cast<char*>(mem::paddrToVaddr(paddr));
      *pch = 'C';
    }
    kout << "we done" << '\n';
  }
  /* DO NOT USE */
}
