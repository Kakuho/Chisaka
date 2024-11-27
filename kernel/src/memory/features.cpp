#include "features.hpp"
#include "firmware/limine/requests.hpp"

namespace Mem{

void PrintMemoryMap(){
  // Procedure to print the memory map at boot.
  //    prints in the format:
  //    paddr \t vaddr \t length \t type
  using namespace limine; // limine::requests
  if(requests::memorymap_request.response == nullptr){
    kout << "MEM FAIL\n";
  }
  std::uint64_t mem_entries_t = requests::memorymap_request.response->entry_count;
  limine_memmap_entry** entries = requests::memorymap_request.response->entries;
  for(std::uint64_t i = 0; i < mem_entries_t; i++){
    // now set the responses
    physaddr_t base = entries[i]->base;
    std::uint64_t length = entries[i]->length;
    std::uint64_t type = entries[i]->type;
    kout << intmode::hex << base << '\t' << PhysToKVirtAddr(base) << '\t'
         << length << '\t' << type << '\n';
  }
}

std::size_t TotalUseableMemory(){
  using namespace limine;
  kassert(requests::memorymap_request.response != nullptr && "MEM FAIL\n");
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

//-------------------------------------------------------------
//  Probers:
//    these functions are more like run time examples
//-------------------------------------------------------------

void ProbeMemory(std::size_t memIndex){
  // probe a single entry, entries[index]
  using namespace limine;
  limine_memmap_entry** entries = requests::memorymap_request.response->entries;
  limine_memmap_entry* region = entries[memIndex];
  kassert(region != nullptr && "ENTRY INDEX IS NULL");
  std::uint64_t type = region->type;
  kassert(type == LIMINE_MEMMAP_USABLE && "REQUIRES USEABLE MEMORY REGION");
  // now we can start probing
  physaddr_t base = region->base;
  std::uint64_t length = region->length;
  kout << "starting to probe memory" << '\n'
       << "max offset should be: " << intmode::hex << base + length << '\n';
  for(physaddr_t paddr = base; paddr < base + length; paddr++){
    char* prober = reinterpret_cast<char*>(PhysToKVirtAddr(paddr));
    *prober = 'A';
    kout << intmode::hex
         << "base = " << base
         << " length = " << length
         << ", paddr = " << paddr
         << ", vaddr = " << PhysToKVirtAddr(paddr)
         << ", value = " << *prober << '\n';
  }
  kout << "DONE!";
}

void ProbeUpperLimit(){
  // procedure to probe the upper limit of usable ram
  using namespace limine;
  limine_memmap_response* memMaps = requests::memorymap_request.response;
  limine_memmap_entry* topRegion = memMaps->entries[memMaps->entry_count - 1];
  // now we can start probing
  std::uint64_t length = topRegion->length;
  physaddr_t base = topRegion->base;
  kout << intmode::dec << "base: " << base << ", length = " << length << '\n';
  // now we do massive probe
  for(physaddr_t paddr = base + length - 50;; paddr++){
    char* pch = reinterpret_cast<char*>(PhysToKVirtAddr(paddr));
    *pch = 'C';
    kout << "paddr = " << paddr << " :: vaddr = " << PhysToKVirtAddr(paddr)
         << " :: *pch = " << *pch << '\n';
  }
}

void ProbeLowerLimit(){
  // procedure to probe the lower limit of usable ram
  using namespace limine;
  limine_memmap_entry* entry = requests::memorymap_request.response->entries[0];
  // now we can start to probe
  std::uint64_t length = entry->length;
  physaddr_t base = entry->base;
  kout << intmode::hex << "base: " << base << ", length = " << length << '\n';
  // do massive probing
  for(physaddr_t paddr = base + 50;; paddr--){
    char* pch = reinterpret_cast<char*>(PhysToKVirtAddr(paddr));
    *pch = 'C';
    kout << "paddr = " << paddr << " :: vaddr = " << PhysToKVirtAddr(paddr)
         << " :: *pch = " << *pch << '\n';
  }
}

//-------------------------------------------------------------
// DO NOT USE
//-------------------------------------------------------------

[[noreturn]] void CorruptMemory(){
  // procedure to completely corrupt the ram >:)
  using namespace limine;
  // first find the last memmap entry
  limine_memmap_response* memMaps = requests::memorymap_request.response;
  std::uint64_t entries = memMaps->entry_count;
  limine_memmap_entry* finalEntry = memMaps->entries[entries -1];
  physaddr_t base_final = finalEntry->base;
  std::size_t length_final = finalEntry->length;
  //kout << intmode::hex << "base: " << base << ", length = " << length << '\n';
  // now we do massive probe - we start from zero to the maximum
  for(physaddr_t paddr = 0; paddr < base_final + length_final; paddr++){
    char* pch = reinterpret_cast<char*>(PhysToKVirtAddr(paddr));
    *pch = 'C';
  }
  // never reaches here
  kout << "we done" << '\n';
}

} // namespace Mem
