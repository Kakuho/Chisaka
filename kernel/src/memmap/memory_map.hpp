#pragma once

// Abstraction to encapsulate the memory mapping. 
//
// Serves as an abstraction and to bookkeep the underlying memory map data 
// obtained from bootloaders such as limine, uefi, or can be mocked by supplying
// user data

#include <cstdint>
#include <initializer_list>

#include "memory_map_entry.hpp"

#include "kcontext.hpp"
#include "types.hpp"
#include "kassert.hpp"

#include "aii/array.hpp"
#include "limine/utility.hpp"
#include "drivers/serial/kostream.hpp"

namespace Chisaka{

class MemoryMap{
  static constexpr std::size_t MAX_ENTRIES = 20;
  public:
    static MemoryMap& Get(){ static MemoryMap memmap; return memmap;}
    void Init() noexcept;
    void Init(std::initializer_list<MemoryMapEntry> src) noexcept;

    constexpr std::size_t TotalUseableMemory() const noexcept{ return m_totalUseableBytes;}

    template<std::size_t PageSize>
    constexpr std::size_t TotalUseablePageFrames() const noexcept{
      return m_totalUseableBytes / PageSize;
    }

    PhysAddr HighestUseableAddress() const noexcept;
    PhysAddr LowestUseableAddress() const noexcept;
    PhysAddr LongestUseableBase() const noexcept;
    std::size_t UseableEntries() const noexcept;
    constexpr std::size_t Entries() const noexcept{ return MAX_ENTRIES;};

    constexpr MemoryMapEntry& 
    Entry(std::size_t index) noexcept{ return m_entries[index];};

    constexpr const MemoryMapEntry& 
    Entry(std::size_t index) const noexcept{ return m_entries[index];};

    void Print() const noexcept;
    void PrintPageFrames() const noexcept;

  private:
    MemoryMap() noexcept = default;
    MemoryMap(std::initializer_list<MemoryMapEntry> src) noexcept;

    void LimineInitialise() noexcept;
    void InitialiseUseableData() noexcept;

  private:
    Aii::Array<MemoryMapEntry, MAX_ENTRIES> m_entries;
    std::size_t m_totalUseableBytes;
};

}
