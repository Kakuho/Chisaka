#pragma once

#include "../registers/control_registers.hpp"
#include "memory/address.hpp"

namespace X8664{

  Mem::physaddr_t GetBaseTable();

  void ParsePageTableCommon();
  
} // namespace X8664
