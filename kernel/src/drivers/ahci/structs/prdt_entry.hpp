#pragma once

// A command table contains a Physical Region Descriptor Table, whose elements is to what will be 
// abstracted

#include <cstdint>
#include <cassert>

#include "kassert.hpp"

namespace Drivers::Ahci{

class PrdtEntry{
  public:

    constexpr PrdtEntry();
    constexpr PrdtEntry(void* dataBase, bool interrupt, std::uint32_t dataSize);

    constexpr std::uint64_t DataBaseAddress() const noexcept{
      std::uint64_t address = m_dbau;
      return (address << 32) | m_dba; 
    }

    constexpr bool InterruptOnCompletion() const noexcept{
      return m_i_dbc >> 31;
    }

    constexpr std::uint32_t DataByteCount() const noexcept{
      return m_i_dbc & 0x3FFFFF;
    }

  public:
    // Dword 0
    std::uint32_t m_dba;
    // Dword 1
    std::uint32_t m_dbau;
    // Dword 2
    std::uint32_t resvered = 0;
    // Dword 3
    std::uint32_t m_i_dbc;

  private:

    void SetDataBaseAddress(void* data);
    constexpr void SetWord3(bool interrupt, std::uint32_t dataSize);
};

static_assert(sizeof(PrdtEntry) == 16);

//-------------------------------------------------------------

constexpr PrdtEntry::PrdtEntry()
  :
    m_dba{0},
    m_dbau{0},
    resvered{0},
    m_i_dbc{0}
{
}

constexpr PrdtEntry::PrdtEntry(void* dataBase, bool interrupt, std::uint32_t dataSize)
  :
    resvered{0}
{
  SetDataBaseAddress(dataBase);
  SetWord3(interrupt, dataSize);
}

void PrdtEntry::SetDataBaseAddress(void* data){
  std::uint64_t addr = reinterpret_cast<std::uint64_t>(data);
  kassert((addr & 0x1) == 0);
  m_dba = addr & 0xFFFF'FFFF;
  m_dbau = addr >> 32;
}

constexpr void PrdtEntry::SetWord3(bool interrupt, std::uint32_t dataSize){
  kassert(dataSize & 0x01);
  m_i_dbc = 0;
  m_i_dbc = dataSize & 0x3'FFFFF;
  if(interrupt){
    m_i_dbc |= 0x8000'0000;
  }
}

}
