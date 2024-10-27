#include "memory.hpp"

namespace memory{
  using namespace limine; // all requests are made using limine::requests
                          
  /*
  void printMemoryMap(){
    // prints in the format:
    // paddr \t vaddr \t length \t type
    if(requests::memorymap_request.response == nullptr){
      kout << "MEM FAIL\n";
    }
    std::uint64_t mem_entries_t = requests::memorymap_request.response->entry_count;
    limine_memmap_entry** entries = requests::memorymap_request.response->entries;
    for(std::uint64_t i = 0; i < mem_entries_t; i++){
      // now set the responses
      paddr64_t base = entries[i]->base;
      std::uint64_t length = entries[i]->length;
      std::uint64_t type = entries[i]->type;
      kout << intmode::hex << base << '\t' << paddrToVaddr(base) << '\t'
           << length << '\t' << type << '\n';
    }
  }
  */

  /*
  void probeMemory(std::size_t index){
    // probe a single entry, entries[index]
    if(requests::memorymap_request.response->entries[index] == nullptr){
      kout << "PROBING FAILED: " << index << '\n';
      return;
    }
    limine_memmap_entry** entries = requests::memorymap_request.response->entries;
    std::uint64_t type = entries[index]->type;
    if(type != LIMINE_MEMMAP_USABLE){
      kout << "THE MEMORY BLOCK ENTRY IS UNUSABLE::TYPE::" << type << "::INDEX::" 
           << index << '\n';
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
  */

  /*
  std::uint64_t calculateAvailableMemory(){
    if(requests::memorymap_request.response == nullptr){
      kout << "MEM FAIL\n";
    }
    std::uint64_t available_memory = 0;
    std::uint64_t mem_entries_t = requests::memorymap_request.response->entry_count;
    limine_memmap_entry** entries = requests::memorymap_request.response->entries;
    for(std::uint64_t i = 0; i < mem_entries_t; i++){
      if(entries[i]->type == LIMINE_MEMMAP_USABLE){
        // only count memory mapping regions which are availabe
        std::uint64_t length = entries[i]->length;
        available_memory += length;
      }
    }
    return available_memory;
  }
  */

  /*
  void printPageFrames(){
    std::uint64_t mem_entries_t = requests::memorymap_request.response->entry_count;
    limine_memmap_entry** entries = requests::memorymap_request.response->entries;
    for(std::uint64_t i = 0; i < mem_entries_t; i++){
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
  */

  /*
  vaddr64_t getKernelVirtualAddress(){
    return requests::kernel_addr_req.response->virtual_base;
  }
  */

  /*
  void upperLimitProbe(){
    // procedure to probe the upper limit of usable ram
    std::uint64_t mem_entries_t = requests::memorymap_request.response->entry_count;
    limine_memmap_entry* entry = requests::memorymap_request.response->entries[mem_entries_t - 1];
    std::uint64_t length = entry->length;
    paddr64_t base = entry->base;
    kout << intmode::dec << "base: " << base << ", length = " << length << '\n';
    // now we do massive probe
    for(paddr64_t paddr = base + length - 50;; paddr++){
      char* pch = reinterpret_cast<char*>(paddrToVaddr(paddr));
      *pch = 'C';
      kout << "paddr = " << paddr << " :: vaddr = " << paddrToVaddr(paddr)
           << " :: *pch = " << *pch << '\n';
    }
  }
  */

/*
  void lowerLimitProbe(){
    // procedure to probe the lower limit of usable ram
    limine_memmap_entry* entry = requests::memorymap_request.response->entries[0];
    std::uint64_t length = entry->length;
    paddr64_t base = entry->base;
    kout << intmode::hex << "base: " << base << ", length = " << length << '\n';
    // now we do massive probe
    for(paddr64_t paddr = base + 50;; paddr--){
      char* pch = reinterpret_cast<char*>(paddrToVaddr(paddr));
      *pch = 'C';
      kout << "paddr = " << paddr << " :: vaddr = " << paddrToVaddr(paddr)
           << " :: *pch = " << *pch << '\n';
    }
  }
*/

  /*
  vaddr64_t FormLinearAddress(std::uint16_t pm4le_i, std::uint16_t pdpte_i, 
      std::uint16_t pde_i, std::uint16_t pt_i, std::uint16_t offset){
    vaddr64_t vaddr = 0;
    // canoncality requirement
    vaddr |= 0xFFFF;
    vaddr <<= 9;
    // pm4le's index
    vaddr |= (pm4le_i & 0x1FF);
    vaddr <<= 9;
    // pdpte's index
    vaddr |= (pdpte_i & 0x1FF);
    vaddr <<= 9;
    // pde's index
    vaddr |= (pde_i & 0x1FF);
    vaddr <<= 9;
    // pt's index
    vaddr |= (pt_i & 0x1FF);
    vaddr <<= 12;
    // the offset
    vaddr |= (offset & 0xFFF);
    return vaddr;
  }
  */

  /*
  void ExtractPagingIndices(vaddr64_t vaddr){
    // a reverse of the FormLinearAddress
    std::uint16_t pageoffset = (vaddr & 0xFFF);
    std::uint16_t pt_i = (vaddr & (0x1FFull << 12)) >> 12;
    std::uint16_t pde_i = (vaddr & (0x1FFull << 21)) >> 21;
    std::uint16_t pdpte_i = (vaddr & (0x1FFull << 30)) >> 30;
    std::uint16_t pm4le_i = (vaddr & (0x1FFull << 39)) >> 39;
    kout << "vaddr :: " << vaddr   << '\n'
         << "page offset :: " << pageoffset << '\n'
         << "pagetable index :: " << pt_i << '\n'
         << "page directory index :: " << pde_i << '\n'
         << "page directory pointer index :: " << pdpte_i << '\n'
         << "pm4l index :: " << pm4le_i << '\n';
  }
  */

  /* DO NOT USE*/
  [[noreturn]] void corruptMemory(){
    // procedure to completely corrupt the ram >:)
    std::uint64_t mem_entries_t = requests::memorymap_request.response->entry_count;
    limine_memmap_entry* entry_final = requests::memorymap_request.response->entries[mem_entries_t -1];
    paddr64_t base_final = entry_final->base;
    paddr64_t length_final = entry_final->length;
    //kout << intmode::hex << "base: " << base << ", length = " << length << '\n';
    // now we do massive probe - we start from zero to the maximum
    for(paddr64_t paddr = 0; paddr < base_final + length_final; paddr++){
      char* pch = reinterpret_cast<char*>(paddrToVaddr(paddr));
      *pch = 'C';
    }
    kout << "we done" << '\n';
  }
  /* DO NOT USE */
}
