#pragma once

#include <cstdint>

#include "./../upper_page_table.hpp"
#include "./../page_table.hpp"
#include "./../utilities.hpp"

#include "drivers/serial/kostream.hpp"

namespace X8664::Samples::ParsePm4l{

inline void Try(){
  Mem::physaddr_t pm4lBase = GetBaseTable();
  kout << intmode::hex << "Pm4L Base :: " << pm4lBase << '\n';
  auto pm4lPtr = reinterpret_cast<UpperPageTable*>(pm4lBase);
  ParseUpperPageTable(*pm4lPtr, PageLevel::Pm4l);

  auto pdptrPtr = static_cast<UpperPageTable*>(pm4lPtr->GetEntry(0x0).TablePtr());
  ParseUpperPageTable(*pdptrPtr, PageLevel::Pdpt);

  auto pdPtr = static_cast<UpperPageTable*>(pdptrPtr->GetEntry(0x0).TablePtr());
  ParseUpperPageTable(*pdPtr, PageLevel::Pd);

  auto ptPtr = static_cast<PageTable*>(pdptrPtr->GetEntry(0x0).TablePtr());
  ParsePageTable(ptPtr);
}

}
