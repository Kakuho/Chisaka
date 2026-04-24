#include "x86pages.hpp"
#include "kcontext.hpp"

namespace X8664{
  auto X86Pages::ExtractPageIndicies(std::uintptr_t virtAddr) -> PageIndices{
    kassert(IsCanoncial(virtAddr));
    std::uint16_t pt_i = (virtAddr & (0x1FFull << 12)) >> 12;
    std::uint16_t pde_i = (virtAddr & (0x1FFull << 21)) >> 21;
    std::uint16_t pdpte_i = (virtAddr & (0x1FFull << 30)) >> 30;
    std::uint16_t pm4le_i = (virtAddr & (0x1FFull << 39)) >> 39;
    return {
      .pm4le = pm4le_i,
      .pdpte = pdpte_i,
      .pde = pde_i,
      .pt = pt_i
    };
  }

  void X86Pages::PrintIndicies(std::uintptr_t address){
    auto indicies = ExtractPageIndicies(address);
    kout << intmode::hex 
         << "pm4le: " << indicies.pm4le << ", left shift 3: " << (indicies.pm4le << 3) << '\n'
         << "pdpte: " << indicies.pdpte << ", left shift 3: " << (indicies.pdpte << 3) << '\n'
         << "pde: " << indicies.pde << ", left shift 3: " << (indicies.pde << 3)       << '\n'
         << "pte: " << indicies.pt << ", left shift 3: " << (indicies.pt << 3)         << '\n';
  }

  std::uintptr_t X86Pages::VirtToPhys(std::uintptr_t virtAddr){
    kassert(IsCanoncial(virtAddr));
    PageIndices index = ExtractPageIndicies(virtAddr);
    if(!m_pml4->GetEntry(index.pm4le).IsPresent()){
      return 0;
    }
    UpperPageTable* pdpt = reinterpret_cast<UpperPageTable*>(m_pml4->GetEntry(index.pm4le).BaseAddress());
    if(!pdpt->GetEntry(index.pdpte).IsPresent()){
      return 0;
    }
    UpperPageTable* pd = reinterpret_cast<UpperPageTable*>(pdpt->GetEntry(index.pdpte).BaseAddress());
    if(!pd->GetEntry(index.pde).IsPresent()){
      return 0;
    }
    PageTable* pt = reinterpret_cast<PageTable*>(pd->GetEntry(index.pde).BaseAddress());
    if(!pt->GetEntry(index.pt).IsPresent()){
      return 0;
    }
    std::uintptr_t pagebase = pt->GetEntry(index.pt).BaseAddress();

    return pagebase + (virtAddr & 0xFFF);
  }

  void X86Pages::MapUserPage(std::uint64_t virtAddr, std::uint64_t pageBase){
    kassert(IsCanoncial(virtAddr));
    kassert((pageBase % 0x1000) == 0);      // is page aligned
    auto& pageAllocator = Chisaka::KContext::PageAllocator::Get();
    PageIndices index = ExtractPageIndicies(virtAddr);
    if(!m_pml4->GetEntry(index.pm4le).IsPresent()){
      void* pml4epage = pageAllocator.AllocatePage();  
      m_pml4->GetEntry(index.pm4le) = UpperPageEntry::CreateUser(pml4epage);
    }
    UpperPageTable* pdpt = reinterpret_cast<UpperPageTable*>(m_pml4->GetEntry(index.pm4le).BaseAddress());
    if(!pdpt->GetEntry(index.pdpte).IsPresent()){
      void* pdptepage = pageAllocator.AllocatePage(); 
      pdpt->GetEntry(index.pdpte) = UpperPageEntry::CreateUser(pdptepage);
    }
    UpperPageTable* pd = reinterpret_cast<UpperPageTable*>(pdpt->GetEntry(index.pdpte).BaseAddress());
    if(!pd->GetEntry(index.pde).IsPresent()){
      void* pdepage = pageAllocator.AllocatePage(); 
      pd->GetEntry(index.pde) = UpperPageEntry::CreateUser(pdepage);
    }
    // finally at the page table, so we can modify the entry
    PageTable* pt = reinterpret_cast<PageTable*>(pd->GetEntry(index.pde).BaseAddress());
    pt->GetEntry(index.pt) = PageEntry::CreateUser(pageBase);
  }
}
