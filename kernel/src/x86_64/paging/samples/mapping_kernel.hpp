#include <cstdint>

#include "kcontext.hpp"
#include "x86_64/x86pages.hpp"

// exploratory code
//
// trying to map 0x0000000000000000 to hhdm start for a new address space
//
// hhdm start is memory address 0xffff800000000000
//
// since this is already mapped in the pml4 given by limine, for new processes, we just need to map pml4[0x800] to
// liminie_pml4[0x800]
//
// we need to map both 0x800 and 0xff8

namespace X8664::Paging::Samples{
  inline void MappingKernel(){
    // you can get more frames by doing page offsets to 0x100000000;
    constexpr std::uintptr_t NEW_PML4_FRAME = 0x100001000;  

    auto ClearPageFrame = [](std::uintptr_t frame){
      memset(reinterpret_cast<void*>(frame), 0x1000, static_cast<char>(0));
    };

    ClearPageFrame(NEW_PML4_FRAME);
    X8664::X86Pages newPages{reinterpret_cast<X8664::UpperPageTable*>(NEW_PML4_FRAME)};

    std::uint64_t cr3 = get_cr3();
    X8664::X86Pages liminePages{reinterpret_cast<X8664::UpperPageTable*>(cr3)};

    newPages.Pml4()->GetEntry(0x100) = PageEntry::CreateSupervisor(liminePages.Pml4()->GetEntry(0x100));
    newPages.Pml4()->GetEntry(0x1ff) = PageEntry::CreateSupervisor(liminePages.Pml4()->GetEntry(0x1ff));

    set_cr3(NEW_PML4_FRAME);
  }
}
