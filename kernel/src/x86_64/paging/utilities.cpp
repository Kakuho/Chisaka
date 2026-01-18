#include "utilities.hpp"
#include "types.hpp"

namespace X8664{

Chisaka::PhysAddr GetBaseTable(){
  // intel vol3a.4.5.4
  static constexpr std::uint64_t MASK = (0xFFFFFFFul << 12);
  std::uint64_t cr3 = get_cr3();
  return cr3 & MASK;
}

void Map( UpperPageTable& uptbl, 
          Chisaka::VirtAddr vbase, 
          Chisaka::PhysAddr pbase, 
          std::size_t gigabytes
        )
{

}

void MapKernel(UpperPageTable& uptbl){
  // Kernel is mapped into 0xffff8000'00000000 -> 0xffff8002'40000000, 
  // and represents a identity mapping of the processors address space
  const auto kernelTbl = KernelPageTable();
  uptbl.AddEntry(0x800, UpperPageEntry{kernelTbl->GetEntry(0).BaseAddress(), 
      UPeOpt::Present
  });
}
  
} // namespace X8664
