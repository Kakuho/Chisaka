#ifndef MEMORY_MAP_DESCRIPTOR_HPP
#define MEMORY_MAP_DESCRIPTOR_HPP

// Abstraction to encapsulate the memory mapping. 
//
// Serves as an abstraction and to bookkeep the underlying memory map data 
// obtained from bootloaders such as limine, uefi, or can be mocked by supplying
// user data

#include <cstdint>
#include <initializer_list>

#include "memory_map_entry.hpp"
#include "./../address.hpp"

#include "primrose/static_array.hpp"
#include "lib/kassert.hpp"
#include "firmware/limine/requests.hpp"
#include "drivers/serial/kostream.hpp"

namespace Mem{

class MemoryMapDescriptor{

static constexpr std::size_t MAX_ENTRIES = 20;

public:
  //-------------------------------------------------------------
  //  Lifetime
  //-------------------------------------------------------------

  MemoryMapDescriptor() noexcept;
  MemoryMapDescriptor(std::initializer_list<MemoryMapEntry> src) noexcept;

  //-------------------------------------------------------------
  //  Queries
  //-------------------------------------------------------------

  [[nodiscard]] constexpr 
  std::size_t TotalUseableMemory() const noexcept{ return m_totalUseableBytes;}

  template<std::size_t PageSize>
  [[nodiscard]] constexpr 
  std::size_t TotalUseablePageFrames() const noexcept{
    return m_totalUseableBytes / PageSize;
  }

  [[nodiscard]] physaddr_t HighestUseableAddress() const noexcept;
  [[nodiscard]] physaddr_t LowestUseableAddress() const noexcept;
  [[nodiscard]] physaddr_t LongestUseableBase() const noexcept;
  [[nodiscard]] std::size_t UseableEntries() const noexcept;

  [[nodiscard]] constexpr std::size_t Entries() const noexcept{ return MAX_ENTRIES;};

  [[nodiscard]] constexpr MemoryMapEntry& 
  Entry(std::size_t index) noexcept{ return m_entries[index];};

  [[nodiscard]] constexpr const MemoryMapEntry& 
  Entry(std::size_t index) const noexcept{ return m_entries[index];};

  //-------------------------------------------------------------
  //  Printing Functionalities
  //-------------------------------------------------------------

  void Print() const noexcept;
  void PrintPageFrames() const noexcept;

private:
  // helpers for lifetime related functions
  void LimineInitialise() noexcept;
  void InitialiseUseableData() noexcept;

private:
  Prim::StaticArray<MemoryMapEntry, MAX_ENTRIES> m_entries;
  std::size_t m_totalUseableBytes;
};

} // namespace Mem

#endif
