#pragma once

// class to represent page tables which contains other page tables as entries

#include <cassert>
#include <cstdint>

#include "constants.hpp"
#include "upper_page_entry.hpp"
#include "page_table.hpp"

#include "aii/array.hpp"

namespace X8664{

enum class PageLevel{
  Pm4l,
  Pdpt,
  Pd
};

class UpperPageTable{
  using EntryType = UpperPageEntry;
  public:
    constexpr UpperPageTable() = default;
    constexpr ~UpperPageTable() = default;

    bool AreEntriesZero() const noexcept;
    void ClearEntries() noexcept;


    constexpr void AddEntry(std::size_t index, 
                            EntryType& entry) noexcept{m_entries[index] = entry;}
    constexpr void AddEntry(std::size_t index, 
                            EntryType&& entry) noexcept{AddEntry(index, entry);}
    constexpr void ClearEntry(std::size_t index) noexcept{m_entries[index] = 0;}

    constexpr EntryType& GetEntry(std::size_t index) noexcept{return m_entries[index];}
    constexpr const EntryType& GetEntry(std::size_t index) const noexcept
    {return m_entries[index];}

    constexpr EntryType& operator[](std::size_t index) noexcept
    {return m_entries[index];}

  private:
    Aii::Array<EntryType, PAGE_TABLE_ENTRIES> m_entries;
}; 
static_assert(sizeof(UpperPageTable) == 4096);

void ParseUpperPageTable( UpperPageTable& table, 
                          PageLevel level);

void ParseUpperPageTable( UpperPageTable& table, 
                          PageLevel level, 
                          bool descend);

} // namespace X8664
