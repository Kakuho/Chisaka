#ifndef CPU_PAGETABLE_UTILITIES_HPP
#define CPU_PAGETABLE_UTILITIES_HPP

// functionalities regarding the paging table for x86_64 processors

#include "../registers/control_registers.hpp"
#include "memory/address.hpp"

namespace X8664::Paging{

  Mem::physaddr_t PagingTableBase();
  
} // namespace X8664::Paging


#endif
