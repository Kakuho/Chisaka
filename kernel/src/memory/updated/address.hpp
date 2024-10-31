#ifndef MEMORY_ADDRESS_HPP
#define MEMORY_ADDRESS_HPP

// memory address related types and functionalities

#include <cstdint>

#include <concepts>

#include "limine/requests.hpp"

namespace Mem{

  // Sensible type declarations
  //  kvirtaddr_t is the kernel's virtual address
  //  (different processes may have different virtual address mappings)

  using physaddr_t = std::uint64_t;
  using kvirtaddr_t = std::uint64_t;

  //-------------------------------------------------------------
  //  Conversions between the physical and virtual addresses
  //-------------------------------------------------------------

  kvirtaddr_t PhysToKVirtAddr(physaddr_t paddr);
  physaddr_t KVirtToPhysAddr(kvirtaddr_t vaddr);

  //-------------------------------------------------------------
  //  Kernel's virtual base address
  //-------------------------------------------------------------

  kvirtaddr_t KernelVirtBase();
  physaddr_t KernelPhysBase();

  //-------------------------------------------------------------
  //  Useful Queries
  //-------------------------------------------------------------
  
  physaddr_t TopUseableAddress();
  physaddr_t BottomUseableAddress();

  //-------------------------------------------------------------
  //  Paging Address Related
  //-------------------------------------------------------------

  struct PageIndices{
      std::uint16_t pm4le;
      std::uint16_t pdpte;
      std::uint16_t pde;
      std::uint16_t pt;
  };

  kvirtaddr_t KVirtFromPageIndex(
      PageIndices&& indices, 
      std::uint16_t offset
  );

  PageIndices ExtractPageIndicies(kvirtaddr_t vaddr);
  void  PrintPageIndicies(kvirtaddr_t vaddr);

  //-------------------------------------------------------------
  //  Page Frame Related
  //-------------------------------------------------------------

  void PrintPageFrames();

} // namespace Mem

#endif
