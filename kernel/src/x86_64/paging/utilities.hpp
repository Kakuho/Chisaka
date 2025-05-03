#pragma once

// functionalities regarding the paging table for x86_64 processors

#include "../registers/control_registers.hpp"
#include "memory/address.hpp"

namespace X8664::Paging{

  Mem::physaddr_t PagingTableBase();

  void ParsePageTableCommon();
  
} // namespace X8664::Paging
