#pragma once

// Frame Descriptor keeps information regarding a singular page frames

#include <cstdint>

#include "memory/address.hpp"
#include "memory/memory_map/memory_map_entry.hpp"

#include "types.hpp"

namespace Mem::Page{

using PageType = Mem::MemoryMapEntry::Type;

class FrameDescriptor{
  enum Flags: std::uint32_t{
    // Usage
    Used = 1 << 1
  };

  public:
    FrameDescriptor(Chisaka::PhysAddr base, PageType type);
    FrameDescriptor(void* base, PageType type): 
      FrameDescriptor(reinterpret_cast<Chisaka::PhysAddr>(base), type){}

    constexpr Chisaka::PhysAddr64 Base() const noexcept{return m_base;}

    constexpr bool IsUsed() const noexcept {return Flags::Used & m_flags;}
    constexpr std::uint32_t RefCount() const noexcept{return m_procCount;}

  private:
    Chisaka::PhysAddr64 m_base;
    PageType m_type;            
    std::uint32_t m_flags;      // 32 bit flag
    std::int32_t m_procCount;
};

}
