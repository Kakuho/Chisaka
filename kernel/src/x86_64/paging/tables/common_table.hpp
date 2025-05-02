#pragma once

// class to represent the other page table types, holding pointers to other pages

#include <cassert>
#include <cstdint>

#include "entries/common_entry.hpp"
#include "aii/array.hpp"

namespace X8664::Paging{

class CommonTable{
  using EntryType = CommonEntry;
  static inline constexpr std::uint16_t ENTRIES = 512;
  public:
    //-------------------------------------------------------------
    //  Lifetime
    //-------------------------------------------------------------

    constexpr CommonTable() = default;
    constexpr ~CommonTable() = default;

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
    Aii::Array<EntryType, ENTRIES> m_entries;
};

static_assert(sizeof(CommonTable) == 4096);

}

