#include "utilites.hpp"
#include "types.hpp"

namespace X8664{

Chisaka::VirtAddr KVirtFromPageIndex(PageIndices&& indices, std::uint16_t offset){
  Chisaka::VirtAddr vaddr = 0;
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

PageIndices ExtractPageIndicies(Chisaka::VirtAddr vaddr){
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

void PrintPageIndicies(Chisaka::VirtAddr vaddr){
  PageIndices indices = ExtractPageIndicies(vaddr);
  kout << "vaddr :: "                         << vaddr   << '\n'
       << "pagetable index :: "               << indices.pt << '\n'
       << "page directory index :: "          << indices.pde << '\n'
       << "page directory pointer index :: "  << indices.pdpte << '\n'
       << "pm4l index :: "                    << indices.pm4le << '\n';
}

}
