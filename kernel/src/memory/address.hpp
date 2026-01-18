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
