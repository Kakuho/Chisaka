#include "address.hpp"

namespace Mem{

//-------------------------------------------------------------
//  Conversions between the physical and virtual addresses
//-------------------------------------------------------------

kvirtaddr_t PhysToKVirtAddr(physaddr_t paddr){
  using namespace limine;
  return paddr + requests::hhdm_request.response->offset;
}

physaddr_t KVirtToPhysAddr(kvirtaddr_t vaddr){
  using namespace limine;
  return vaddr - requests::hhdm_request.response->offset;
}

//-------------------------------------------------------------
//  Kernel's virtual base address
//-------------------------------------------------------------

kvirtaddr_t KernelVirtBase(){
  using namespace limine;
  return requests::kernel_addr_req.response->virtual_base;
}

kvirtaddr_t KernelPhysBase(){
  using namespace limine;
  return requests::kernel_addr_req.response->physical_base;
}

//-------------------------------------------------------------
//  Useful Queries
//-------------------------------------------------------------

physaddr_t TopUseableAddress(){
  // procedure to get the top limit of usable ram
  using namespace limine;
  limine_memmap_response* memMaps = requests::memorymap_request.response;
  // find the top most usable memory region
  std::uint16_t count = 0;
  auto topEntry = memMaps->entries[memMaps->entry_count - count];
  while(topEntry -> type != LIMINE_MEMMAP_USABLE){
    count++;
    topEntry = memMaps->entries[memMaps->entry_count - count];
  }
  std::uint64_t length = topEntry->length;
  physaddr_t base = topEntry->base;
  return base + length;
}

physaddr_t BottomUseableAddress(){
  // procedure to get the bottom limit of usable ram
  using namespace limine;
  limine_memmap_response* memMaps = requests::memorymap_request.response;
  // find the bottom most usable memory region
  std::uint16_t count = 0;
  auto bottomEntry = memMaps->entries[count];
  while(bottomEntry -> type != LIMINE_MEMMAP_USABLE){
    count++;
    bottomEntry = memMaps->entries[count];
  }
  physaddr_t base = bottomEntry->base;
  return base;
}

//-------------------------------------------------------------
//  Paging Address Related
//-------------------------------------------------------------

kvirtaddr_t KVirtFromPageIndex(
  PageIndices&& indices, 
  std::uint16_t offset
){
  kvirtaddr_t vaddr = 0;
  // canoncality requirement
  vaddr |= 0xFFFF;
  vaddr <<= 9;
  // pm4le's index
  vaddr |= (indices.pm4le & 0x1FF);
  vaddr <<= 9;
  // pdpte's index
  vaddr |= (indices.pdpte & 0x1FF);
  vaddr <<= 9;
  // pde's index
  vaddr |= (indices.pde & 0x1FF);
  vaddr <<= 9;
  // pt's index
  vaddr |= (indices.pt & 0x1FF);
  vaddr <<= 12;
  // the offset
  vaddr |= (offset & 0xFFF);
  return vaddr;
}

PageIndices ExtractPageIndicies(kvirtaddr_t vaddr){
  std::uint16_t pt_i = (vaddr & (0x1FFull << 12)) >> 12;
  std::uint16_t pde_i = (vaddr & (0x1FFull << 21)) >> 21;
  std::uint16_t pdpte_i = (vaddr & (0x1FFull << 30)) >> 30;
  std::uint16_t pm4le_i = (vaddr & (0x1FFull << 39)) >> 39;
  return {
    .pm4le = pm4le_i,
    .pdpte = pdpte_i,
    .pde = pde_i,
    .pt = pt_i
  };
}

void PrintPageIndicies(kvirtaddr_t vaddr){
  PageIndices indices = ExtractPageIndicies(vaddr);
  kout << "vaddr :: "                         << vaddr   << '\n'
       << "pagetable index :: "               << indices.pt << '\n'
       << "page directory index :: "          << indices.pde << '\n'
       << "page directory pointer index :: "  << indices.pdpte << '\n'
       << "pm4l index :: "                    << indices.pm4le << '\n';
}

//-------------------------------------------------------------
//  Page Frame Related
//-------------------------------------------------------------

void PrintPageFrames(){
  // Prints in the format (index, page, length)
  using namespace limine;
  std::uint64_t mem_entries_t = requests::memorymap_request.response->entry_count;
  limine_memmap_entry** entries = requests::memorymap_request.response->entries;
  std::size_t index = 0;
  for(std::uint64_t i = 0; i < mem_entries_t; i++){
    // only count memory mapping regions which are availabe
    if(entries[i]->type == LIMINE_MEMMAP_USABLE){
      std::uint64_t base = entries[i]->base;
      std::uint64_t length = entries[i]->length;
      for(physaddr_t paddr = base; paddr < base + length; paddr += 0x1000){
        // n.b 0x1000 = 4096 = page size
        kout << "(" << index++  << ", " 
                    << paddr    << ", " 
                    << (paddr + 0x1000) - 1 
                    << ")" << '\n';
      }
    }
  }
}

} // namespace Mem
