#include "pages.hpp"
#include "cpu/utilites.hpp"

namespace memory{
  constexpr std::size_t availablePages = 0x1fc000;
  paddr64_t startOfRam = 0;
  std::array<page, availablePages> frameMap;
  std::size_t nextFrame;

  void InitialisePageFrames(){
    using namespace limine;
    kout << "initiaialising Pageframes... " << '\n';
    std::uint8_t initialBlockIndex = 0;
    std::size_t frameIndex = 0;
    std::uint64_t mem_entries_t = requests::memorymap_request.response->entry_count;
    limine_memmap_entry** entries = requests::memorymap_request.response->entries;
    for(std::uint64_t i = 0; i < mem_entries_t; i++){
      // only count memory mapping regions which are availabe
      if(entries[i]->type == LIMINE_MEMMAP_USABLE){
        if(initialBlockIndex == 0){
          initialBlockIndex = i;
        }
        paddr64_t base = entries[i]->base;
        std::size_t length = entries[i]->length;
        kout << "usable chunk with base :: " << base << '\n';
        for(paddr64_t paddr = base; paddr < base + length; paddr += 0x1000){
          // populating page frame descriptors
          frameMap[frameIndex].base = paddr;
          frameMap[frameIndex].flags = 0;
          frameMap[frameIndex].count = -1;
          /*
          kout << "populate page frame :: " << frameIndex << " :: with values :: " <<
               frameMap[frameIndex].base << '\n';
          */
          if(++frameIndex == availablePages){
            break;
          }
        }
        if(i == initialBlockIndex){
          startOfRam = base;
          kout << "start of ram :: " << startOfRam << '\n';
        }
        kout << "finished archiving pages with base :: " << base << '\n';
      }
    }
    kout << "finished initialising page frames :: total :: " << GetAvailablePages() 
         << " / " << GetBootPages() << '\n';
  }

  void PrintSizeofPagetorMap(){
    kout << "sizeof page frame descriptor map: " << sizeof(frameMap) << '\n';
  }

  void PrintAvailablePages(){
    kout << "Available Pages at boot " << GetAvailablePages() << '\n';
  }

  std::size_t GetAvailablePages(){
    return frameMap.size();
  }

  void CheckBootPages(){
    if(memory::GetAvailablePages() > memory::GetBootPages()){
      kout << "ERROR:: number of pages tracked < number of available pages" << '\n';
      X8664::HaltCatchFire();
    }
  }

  void PrintPageFrames(){
    for(std::size_t i = 0; i < availablePages; i++){
      kout << "page frame index :: " << i << " :: " 
           << " base address :: " << frameMap[i].base << " :: " 
           << (frameMap[i].count < 0 ? " free " : " not in use ") 
           << '\n';
    }
  }

  paddr64_t PageAddress(std::size_t frameNumber){
    return frameMap[frameNumber].base;
  }

  paddr64_t VirtualToPageBase(vaddr64_t vaddr){
    kout << "vaddr :: " << vaddr << '\n';
    std::uint16_t pte_i =   (vaddr & (0x1FFull << 12)) >> 12;
    std::uint16_t pde_i =   (vaddr & (0x1FFull << 21)) >> 21;
    std::uint16_t pdpte_i = (vaddr & (0x1FFull << 30)) >> 30;
    std::uint16_t pm4le_i = (vaddr & (0x1FFull << 39)) >> 39;
    using namespace paging;
    paddr64_t pm4laddr = GetCurrentPM4L();
    // ----
    P4eDescriptor* p4e = reinterpret_cast<P4eDescriptor*>(pm4laddr + 8*pm4le_i);
    kout << "p4e's reference: " << reinterpret_cast<paddr64_t>(p4e->address()) << '\n';
    // ----
    PdpteDescriptor* pdpte = reinterpret_cast<PdpteDescriptor*>(p4e->address() + 8*pdpte_i);
    kout << "pdpte's reference: " << reinterpret_cast<paddr64_t>(pdpte->address()) << '\n';
    // ----
    PdeDescriptor* pde = reinterpret_cast<PdeDescriptor*>(pdpte->address() + 8*pde_i);
    kout << "pde's reference: " << reinterpret_cast<paddr64_t>(pde->address()) << '\n';
    // ----
    PteDescriptor* pte = reinterpret_cast<PteDescriptor*>(pde->address() + 8*pte_i);
    kout << "pte's reference: " << reinterpret_cast<paddr64_t>(pte->address()) << '\n';
    // ----
    return pte->address();
  }

}
