#include "upper_page_table.hpp"

namespace X8664{

bool X8664::UpperPageTable::AreEntriesZero() const{
  for(std::size_t i = 0; i < ENTRIES; i++){
    if(m_entries[i] != 0)
      return false;
  }
  return true;
}

void X8664::UpperPageTable::ClearEntries(){
  for(std::size_t i = 0; i < ENTRIES; i++){
    m_entries[i] = 0;
  }
}

}
