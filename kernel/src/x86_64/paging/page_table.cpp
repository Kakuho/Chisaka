#include "page_table.hpp"

namespace X8664{

bool PageTable::AreEntriesZero() const{
  for(std::size_t i = 0; i < PAGE_TABLE_ENTRIES; i++){
    if(m_entries[i] != 0)
      return false;
  }
  return true;
}

void PageTable::ClearEntries(){
  for(std::size_t i = 0; i < PAGE_TABLE_ENTRIES; i++){
    m_entries[i] = 0;
  }
}

void ParsePageTable(PageTable& pt) noexcept{
  for(std::uint16_t j = 0; j < PAGE_TABLE_ENTRIES; j++){
  PageEntry& entry = pt[j];
  kout << "PT[ "<< j << "] :: "; entry.PrintValues();
  }
}

}
