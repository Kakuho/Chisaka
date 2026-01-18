#ifndef MEMORY_ADDRESS_HPP
#define MEMORY_ADDRESS_HPP

// memory address related types and functionalities

#include <cstdint>
#include <concepts>

#include "limine/utility.hpp"

namespace Mem{

  using kvirtaddr_t = std::uint64_t;

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

} // namespace Mem

#endif
