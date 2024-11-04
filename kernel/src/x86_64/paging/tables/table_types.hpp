#ifndef X8664_TABLE_TYPES_HPP
#define X8664_TABLE_TYPES_HPP

// Type aliases class for the upper level page tables.

#include <cassert>
#include <cstdint>

#include "common_table.hpp"
#include "entries/common_entry.hpp"
#include "primrose/static_array.hpp"

namespace X8664::Paging{
  
using Pm4lTable = CommonTable;
using PdptTable = CommonTable;
using PdTable   = CommonTable;

static_assert(sizeof(Pm4lTable) == 4096);
static_assert(sizeof(PdptTable) == 4096);
static_assert(sizeof(PdTable) == 4096);

}

#endif
