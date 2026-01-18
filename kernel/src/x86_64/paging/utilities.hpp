#pragma once

#include "upper_page_table.hpp"

#include "../registers/control_registers.hpp"
#include "kassert.hpp"

#include "types.hpp"

namespace X8664{

Chisaka::PhysAddr GetBaseTable();

void ParsePageTableCommon();

void Map( UpperPageTable& uptbl, 
          Chisaka::VirtAddr vbase, 
          Chisaka::PhysAddr pbase, 
          std::size_t gigabytes
        );

void MapKernel( UpperPageTable& uptbl);

inline UpperPageTable* KernelPageTable()
{ kassert(false && "KernelPageTable() not impl"); }

} // namespace X8664
