#pragma once

// class to represent page tables which contains other page tables as entries

#include <cassert>
#include <cstdint>

#include "upper_page_entry.hpp"
#include "aii/array.hpp"

namespace X8664{

class UpperPageTable{
  using EntryType = UpperPageEntry;
  static inline constexpr std::uint16_t ENTRIES = 512;

  public:
    constexpr UpperPageTable() = default;
    constexpr ~UpperPageTable() = default;

    bool AreEntriesZero() const;
    void ClearEntries();

    constexpr void AddEntry(std::size_t index, 
                            EntryType& entry){m_entries[index] = entry;}
    constexpr void AddEntry(std::size_t index, 
                            EntryType&& entry){AddEntry(index, entry);}
    constexpr void ClearEntry(std::size_t index){m_entries[index] = 0;}

    constexpr EntryType& operator[](std::size_t index){return m_entries[index];}

  private:
    Aii::Array<EntryType, ENTRIES> m_entries;
};

static_assert(sizeof(UpperPageTable) == 4096);

}
