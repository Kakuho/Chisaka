#ifndef X8664_PAGE_TABLE_HPP
#define X8664_PAGE_TABLE_HPP

// class to represent a page table, holding entries to pages

#include <cassert>
#include <cstdint>

#include "entries/pagetable_entry.hpp"
#include "primrose/static_array.hpp"

namespace X8664::Paging{

class PageTable{
  using EntryType = PageTableEntry;
  static inline constexpr std::uint16_t ENTRIES = 512;
  public:
    //-------------------------------------------------------------
    //  Lifetime
    //-------------------------------------------------------------

    constexpr PageTable() = default;
    constexpr ~PageTable() = default;

    //-------------------------------------------------------------
    //  Operations:
    //    Mapping, Unmapping, Queries etc
    //-------------------------------------------------------------

    constexpr bool IsEntriesZero() const{
      for(std::size_t i = 0; i < ENTRIES; i++){
        if(m_entries[i] != 0)
          return false;
      }
      return true;
    }

    constexpr void ClearEntries(){
      for(std::size_t i = 0; i < ENTRIES; i++){
        m_entries[i] = 0;
      }
    }

    constexpr void AddEntry(std::size_t index, EntryType& entry){
      m_entries[index] = entry;
    }

    constexpr void AddEntry(std::size_t index, EntryType&& entry){
      AddEntry(index, entry);
    }

    constexpr void ClearEntry(std::size_t index){
      m_entries[index] = 0;
    }

    //-------------------------------------------------------------
    //  Operator Overloads
    //-------------------------------------------------------------

    constexpr EntryType& operator[](std::size_t index){ 
      return m_entries[index];
    }

  private:
    Prim::StaticArray<EntryType, ENTRIES> m_entries;
};

static_assert(sizeof(PageTable) == 4096);

}

#endif
