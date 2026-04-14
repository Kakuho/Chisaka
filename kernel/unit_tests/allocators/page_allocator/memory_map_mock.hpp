#pragma once

#include <initializer_list>
#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iomanip>

namespace Chisaka::Tests{
  class MemoryMapMock{
    std::size_t PAGESIZE = 0x1000;
    enum class EntryType{Useable, Unuseable};

    struct EntryInit_t{
      EntryType type;
      std::uint64_t length;
    };

    public:
      struct Entry_t{
        std::uint64_t base;
        std::uint64_t length;
      };

      static MemoryMapMock& Get(){ static MemoryMapMock memmap; return memmap;}

      void Init() noexcept; // the original layout
      void InitFlat(std::size_t nbytes) noexcept;
      void InitDiscontigous(std::initializer_list<EntryInit_t> entryInits) noexcept;         

      constexpr std::size_t TotalUseableMemory() const noexcept{ return m_totalUseableBytes;}
      constexpr std::size_t TotalUseablePageFrames() const noexcept{
        return m_totalUseableBytes / PAGESIZE;
      }

      void DumpMemory(const std::string& filename) const;

    private:
      std::vector<std::uint8_t> m_buffer;
      std::vector<Entry_t> m_entries;
      std::size_t m_totalUseableBytes;
  };
}
