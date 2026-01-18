#ifndef MEMORY_ADDRESS_HPP
#define MEMORY_ADDRESS_HPP

// memory address related types and functionalities

#include <cstdint>
#include <concepts>

#include "limine/utility.hpp"

namespace Mem{

  // Sensible type declarations
  //  kvirtaddr_t is the kernel's virtual address
  //  (different processes may have different virtual address mappings)

  //using physaddr64_t = std::uint64_t;
  //using physaddr32_t = std::uint32_t;
  using physaddr_t = std::uint64_t;     // catch all

  using kvirtaddr_t = std::uint64_t;

  template<typename T>
  concept IsAddressType = (
                            std::same_as<T, physaddr_t> || 
                            std::same_as<T, kvirtaddr_t>
                          );

  template<typename T>
  concept NotIntegralAddressType = ( !IsAddressType<T> );

  //-------------------------------------------------------------
  //  Conversions between address types
  //-------------------------------------------------------------

  kvirtaddr_t PhysToKVirtAddr(physaddr_t paddr);
  physaddr_t KVirtToPhysAddr(kvirtaddr_t vaddr);

  template<typename T>
    requires IsAddressType<T>
  constexpr void* PVAddrToPtr(T addr){
    return reinterpret_cast<void*>(addr);
  }

  template<typename T>
    requires IsAddressType<T>
  constexpr T PtrToAddr(void* ptr){
    return reinterpret_cast<T>(ptr);
  }

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

  struct BaseLength{
    physaddr_t base;
    std::size_t length;
  };

  // in physical
  BaseLength LongestBaseLength();

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
