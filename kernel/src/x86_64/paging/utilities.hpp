#pragma once

#include "upper_page_table.hpp"

#include "../registers/control_registers.hpp"
#include "memory/address.hpp"
#include "kassert.hpp"

namespace X8664{

Mem::physaddr_t GetBaseTable();

void ParsePageTableCommon();

void Map( UpperPageTable& uptbl, 
          Mem::kvirtaddr_t vbase, 
          Mem::physaddr_t pbase, 
          std::size_t gigabytes
        );

void MapKernel( UpperPageTable& uptbl);

inline UpperPageTable* KernelPageTable()
{ kassert(false && "KernelPageTable() not impl"); }


} // namespace X8664
