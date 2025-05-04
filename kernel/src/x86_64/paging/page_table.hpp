#pragma once

// class to represent a page table, holding entries to pages

#include <cassert>
#include <cstdint>

#include "page_entry.hpp"
#include "constants.hpp"

#include "aii/array.hpp"

namespace X8664{

class PageTable{
  using EntryType = PageEntry;
  public:
    constexpr PageTable() = default;
    constexpr ~PageTable() = default;

    bool AreEntriesZero() const;
    void ClearEntries();

    constexpr void AddEntry(std::size_t index, 
                            EntryType& entry) noexcept{m_entries[index] = entry;}
    constexpr void AddEntry(std::size_t index, 
                            EntryType&& entry) noexcept{AddEntry(index, entry);}
    constexpr void ClearEntry(std::size_t index) noexcept{m_entries[index] = 0;}

    constexpr EntryType& operator[](std::size_t index)
    {return m_entries[index];}

  private:
    Aii::Array<EntryType, PAGE_TABLE_ENTRIES> m_entries;
};
static_assert(sizeof(PageTable) == 4096);

void ParsePageTable(PageTable& pt) noexcept;
inline void ParsePageTable(PageTable* pt) noexcept{ ParsePageTable(*pt); }

}
