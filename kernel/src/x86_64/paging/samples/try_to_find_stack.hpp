#include "kcontext.hpp"
#include "x86_64/x86pages.hpp"

// exploratory code, trying to find the page table of a stack allocated variable, failing though :(

namespace X8664::Paging::Samples{
  inline void TryToFindStack(){
    Chisaka::KContext::Get().InitKContext();
    Chisaka::KContext::Memmap::Get().Init();

    auto& pageAllocator = Chisaka::KContext::PageAllocator::Get();
    pageAllocator.Init();

    std::uint64_t cr3 = get_cr3();
    kout << intmode::hex << "Cr3: " << cr3 << '\n';

    X8664::X86Pages pagetable{reinterpret_cast<X8664::UpperPageTable*>(cr3)};
    kassert(pagetable.Pml4()->AreEntriesZero() == false);
    kout << static_cast<std::uint64_t>(pagetable.Pml4()->GetEntry(0)) << '\n';
    
    // the stack allocated variable we want to page fault on
    int x = 11;
    std::uintptr_t xaddr = reinterpret_cast<std::uintptr_t>(&x);
    auto xindicies = pagetable.ExtractPageIndicies(xaddr);

    kout << "x address: " << xaddr << '\n';
    kout << "x physical: " << Chisaka::KContext::Get().VirtToPhysAddr(xaddr) << '\n';
    kout << intmode::hex
         << " pml4 index: " << xindicies.pm4le << ", left shift 3" << (xindicies.pm4le << 3)<< '\n'
         << " pdepte index: " << xindicies.pdpte << ", left shift 3" << (xindicies.pdpte << 3)<< '\n'
         << " pde index: " << xindicies.pde << ", left shift 3" << (xindicies.pde << 3)<< '\n'
         << " pt index: " << xindicies.pt << ", left shift 3" << (xindicies.pt << 3)<< '\n';

    //pagetable.Pml4()->GetEntry(0x100).SetPresent(false);
    //pagetable.Pml4()->ClearEntry(0x100);
    //pagetable.Pml4()->ClearEntry(0x0);
    kout << "ptl4e : " << static_cast<std::uint64_t>(pagetable.Pml4()->GetEntry(xindicies.pm4le)) << "\ntable address: " << pagetable.Pml4()->GetEntry(xindicies.pm4le).BaseAddress() << '\n';

    X8664::UpperPageTable* pdpt = reinterpret_cast<X8664::UpperPageTable*>(pagetable.Pml4()->GetEntry(xindicies.pm4le).BaseAddress());
    //pdpt->GetEntry(xindicies.pdpte).SetPresent(false);
    //pdpt->ClearEntry(xindicies.pdpte);
    kout << "pdpte : " << static_cast<std::uint64_t>(pdpt->GetEntry(xindicies.pdpte)) << "\ntable address: " << pdpt->GetEntry(xindicies.pdpte).BaseAddress() << '\n';


    X8664::UpperPageTable* pd = reinterpret_cast<X8664::UpperPageTable*>(pdpt->GetEntry(xindicies.pdpte).BaseAddress());
    //pd->ClearEntry(xindicies.pde);
    //InvalidateTlb();
    kout << "pde : " << static_cast<std::uint64_t>(pd->GetEntry(xindicies.pde)) << "\ntable address: " << pd->GetEntry(xindicies.pde).BaseAddress() << '\n';

    // trying to see non zero entries

    X8664::PageTable* pt = reinterpret_cast<X8664::PageTable*>(pd->GetEntry(xindicies.pde).BaseAddress());

    pt->ClearEntry(xindicies.pt);
    kout << "pte : " << static_cast<std::uint64_t>(pt->GetEntry(xindicies.pt)) << "\npage base address: " << pt->GetEntry(xindicies.pt).BaseAddress() << '\n';

    // this should page fault because we cleared the page table entry of pt
    x = 234;

    kout << pagetable.VirtToPhys(xaddr) << '\n';
  }

  inline void TryToFindX(std::uintptr_t xaddr){
    // pml4 index 0 doesn't have it
    // pml4 index 100 && pdpt index 0 kinda dies idk why
    std::uint64_t cr3 = get_cr3();
    kout << intmode::hex << "Cr3: " << cr3 << '\n';
    X8664::X86Pages pagetable{reinterpret_cast<X8664::UpperPageTable*>(cr3)};
    for(int i = 100; i < 512; i++){
      if(!pagetable.Pml4()->GetEntry(i).IsPresent()){
        continue;
      }
      X8664::UpperPageTable* pdpt =
        reinterpret_cast<X8664::UpperPageTable*>(pagetable.Pml4()->GetEntry(i).BaseAddress());
      for(int j = 1; j < 512; j++){
        if(!pdpt->GetEntry(j).IsPresent()){
          continue;
        }
        X8664::UpperPageTable* pd =
          reinterpret_cast<X8664::UpperPageTable*>(pdpt->GetEntry(j).BaseAddress());
        for(int k = 0; k < 512; k++){
          if(!pd->GetEntry(k).IsPresent()){
            continue;
          }
          X8664::UpperPageTable* pt =
            reinterpret_cast<X8664::UpperPageTable*>(pd->GetEntry(k).BaseAddress());
          for(int l = 0 ;l < 512; l++){
            kout << "pml4e index " << i << " pdpte index: " << j
                 << " pde index: " << k << " pte index: " << l << '\n';
            auto entry = pt->GetEntry(l);
            if(entry.BaseAddress() == 0x7FF54000){
              kassert(false && "misketa");
            }
          }
        }
      }
    }
  }
}
