#include <cstdint>

#include "kcontext.hpp"
#include "x86_64/x86pages.hpp"

// exploratory code
//
// does a higher page table entry being present, override a lower entry being not present?
//
// Ans: the answer is no, the lower page table needs to have present entries too in order for the linear/virtual address to 
//      be valid

namespace X8664::Paging::Samples{
  inline void ClearPageFrame(std::uintptr_t frame){
    memset(reinterpret_cast<void*>(frame), 0x1000, static_cast<char>(0));
  }

  inline void HigherTableOverride(){
    constexpr std::uintptr_t USEABLE_FRAME0 = 0x100000000;  // pdpt
    constexpr std::uintptr_t USEABLE_FRAME1 = 0x100001000;  // pd
    constexpr std::uintptr_t USEABLE_FRAME2 = 0x100002000;  // pt
    constexpr std::uintptr_t USEABLE_FRAME3 = 0x100003000;

    auto memmap = Chisaka::KContext::Memmap::Get();
    memmap.Init();
    memmap.Print();

    std::uint64_t cr3 = get_cr3();
    kout << intmode::hex << "Cr3: " << cr3 << '\n';

    X8664::X86Pages pagetable{reinterpret_cast<X8664::UpperPageTable*>(cr3)};
    kassert(pagetable.Pml4()->AreEntriesZero() == false);
    kout << static_cast<std::uint64_t>(pagetable.Pml4()->GetEntry(0)) << '\n';

    ClearPageFrame(USEABLE_FRAME0);
    pagetable.Pml4()->GetEntry(0x104) = X8664::PageEntry::CreateSupervisor(USEABLE_FRAME0);
    pagetable.Pml4()->GetEntry(0x104).SetPresent(true);
    
    ClearPageFrame(USEABLE_FRAME1);
    X8664::UpperPageTable* pdpt = reinterpret_cast<X8664::UpperPageTable*>(USEABLE_FRAME0);
    pdpt->GetEntry(0x01) = X8664::PageEntry::CreateSupervisor(USEABLE_FRAME1);
    pdpt->GetEntry(0x01).SetPresent(true);

    ClearPageFrame(USEABLE_FRAME2);
    X8664::UpperPageTable* pd = reinterpret_cast<X8664::UpperPageTable*>(USEABLE_FRAME1);
    pd->GetEntry(0x01) = X8664::PageEntry::CreateSupervisor(USEABLE_FRAME2);
    pd->GetEntry(0x01).SetPresent(true);

    ClearPageFrame(USEABLE_FRAME3);
    X8664::PageTable* pt = reinterpret_cast<X8664::PageTable*>(USEABLE_FRAME2);
    pt->GetEntry(0x01) = X8664::PageEntry::CreateSupervisor(USEABLE_FRAME3);
    pt->GetEntry(0x01).SetPresent(false);

    std::uintptr_t addr = 0xFFFF820040201000;
    auto indicies = pagetable.ExtractPageIndicies(addr);

    kout << "addr: " << addr << '\n';
    kout << "addr physical: " << Chisaka::KContext::Get().VirtToPhysAddr(addr) << '\n';
    kout << intmode::hex
         << " pml4 index: " << indicies.pm4le << ", left shift 3" << (indicies.pm4le << 3)<< '\n'
         << " pdepte index: " << indicies.pdpte << ", left shift 3" << (indicies.pdpte << 3)<< '\n'
         << " pde index: " << indicies.pde << ", left shift 3" << (indicies.pde << 3)<< '\n'
         << " pt index: " << indicies.pt << ", left shift 3" << (indicies.pt << 3)<< '\n';

    std::uint32_t* ptr = reinterpret_cast<std::uint32_t*>(addr);
    *ptr = 0xDEAD;
  }
}
