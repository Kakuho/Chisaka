#include "upper_page_table.hpp"

namespace X8664{

bool UpperPageTable::AreEntriesZero() const noexcept{
  for(std::size_t i = 0; i < PAGE_TABLE_ENTRIES; i++){
    if(m_entries[i] != 0)
      return false;
  }
  return true;
}

void UpperPageTable::ClearEntries() noexcept{
  for(std::size_t i = 0; i < PAGE_TABLE_ENTRIES; i++){
    m_entries[i] = 0;
  }
}

void ParseUpperPageTable(UpperPageTable& table, PageLevel level){
  for(std::uint16_t j = 0; j < PAGE_TABLE_ENTRIES; j++){
    UpperPageEntry& entry = table[j];
    switch(level){
      case PageLevel::Pm4l:
        kout << "PM4L[ "<< j << "] :: "; entry.PrintValues();
        break;
      case PageLevel::Pdpt:
        kout << "PDPT[ "<< j << "] :: "; entry.PrintValues();
        break;
      case PageLevel::Pd:
        kout << "PD[ "<< j << "] :: "; entry.PrintValues();
        break;
    }
  }
}

void ParseUpperPageTable( UpperPageTable& table, 
                          PageLevel level, 
                          bool descend)
{
  if(descend == true){
    for(std::uint16_t j = 0; j < PAGE_TABLE_ENTRIES; j++){
      UpperPageEntry& entry = table[j];
      switch(level){
        case PageLevel::Pm4l:
          kout << "PM4L[ "<< j << "] :: "; entry.PrintValues();
          ParseUpperPageTable(*reinterpret_cast<UpperPageTable*>(entry.BaseAddress()), 
                              PageLevel::Pdpt, true);
          break;
        case PageLevel::Pdpt:
          kout << "PDPT[ "<< j << "] :: "; entry.PrintValues();
          ParseUpperPageTable(*reinterpret_cast<UpperPageTable*>(entry.BaseAddress()), 
                              PageLevel::Pd, true);
          break;
        case PageLevel::Pd:
          kout << "PD[ "<< j << "] :: "; entry.PrintValues();
          ParsePageTable(reinterpret_cast<PageTable*>(entry.BaseAddress()));
          break;
        default:
          return;
      }
    }
  }
  else{
    ParseUpperPageTable(table, level);
  }
}

void DoSomething(){
  UpperPageTable pt{};
  pt[0] = 0x1111;
  pt[0].PrintValues();
}

}
