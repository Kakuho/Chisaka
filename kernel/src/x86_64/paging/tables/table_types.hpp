#pragma once

// Type aliases class for the upper level page tables.

#include <cassert>
#include <cstdint>

#include "upper_page_table.hpp"
#include "upper_page_entry.hpp"
#include "aii/array.hpp"

namespace X8664{
  
using Pm4lTable = UpperPageTable;
using PdptTable = UpperPageTable;
using PdTable   = UpperPageTable;

static_assert(sizeof(Pm4lTable) == 4096);
static_assert(sizeof(PdptTable) == 4096);
static_assert(sizeof(PdTable)   == 4096);

}
