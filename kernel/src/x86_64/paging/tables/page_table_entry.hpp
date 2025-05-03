#pragma once

//  Only the lowest level Page Table Entry have a different shape compared to the 
//  other table entries
//
//  Reference: Intel Vol3.4.5

#include <cstdint>
#include <cassert>

#include "memory/alignment.hpp"
#include "memory/address.hpp"

namespace X8664{

// Helper flag modifier for page table entry attributes

enum PEOpt: std::uint16_t{
  Present = 1 << 0,
  Writeable = 1 << 1,
  UserAccessible = 1 << 2,
  CacheWritethrough = 1 << 3,
  CacheDisable = 1 << 4,
  Accessed = 1 << 5,
  Dirty = 1 << 6,
  Pat = 1 << 7,
  Global = 1 << 8,
};

class PageTableEntry{
  using VirtAddr_t = std::uint64_t;
  public:
    constexpr explicit PageTableEntry() noexcept: m_buffer{0}{}
    constexpr explicit PageTableEntry(VirtAddr_t address, 
                                      std::uint8_t flags) noexcept;
    constexpr explicit PageTableEntry(std::uint64_t src) noexcept: m_buffer{src}{}

    constexpr const PageTableEntry& operator=(std::uint64_t src);
    constexpr operator std::uint64_t() noexcept{ return m_buffer;}
    constexpr operator std::uint64_t() const noexcept{ return m_buffer;}

    constexpr bool IsPresent() const noexcept;
    constexpr void SetPresent(bool b) noexcept;

    constexpr bool IsWriteable() const noexcept;
    constexpr void SetWriteable(bool b) noexcept;

    constexpr bool IsUserAccessible() const noexcept;
    constexpr void SetUserAccessible(bool b) noexcept;

    constexpr bool IsPwt() const noexcept;
    constexpr void SetPwt(bool b) noexcept;

    constexpr bool IsPcd() const noexcept;
    constexpr void SetPcd(bool b) noexcept;

    constexpr bool IsAccessed() const noexcept;
    constexpr void SetAccessed(bool b) noexcept;

    constexpr bool IsDirty() const noexcept;
    constexpr void SetDirty(bool b) noexcept;

    constexpr bool IsGlobal() const noexcept;
    constexpr void SetGlobal(bool b) noexcept;

  private:
    constexpr void InitialiseBuffer(VirtAddr_t address, std::uint8_t flags) noexcept;
    constexpr void SetAttribute(bool val, PEOpt modifier) noexcept;

  private:
    std::uint64_t m_buffer;
};

static_assert(sizeof(PageTableEntry) == 8);

} // namespace X8664::Paging

// Impl

constexpr X8664::PageTableEntry::PageTableEntry(VirtAddr_t address, 
                                                std::uint8_t flags) noexcept
{
  InitialiseBuffer(address, flags);
}

constexpr void 
X8664::PageTableEntry::InitialiseBuffer(VirtAddr_t address, std::uint8_t flags) noexcept{
  // assumption: address is a kernel virtual address
  m_buffer = 0;
  m_buffer |= flags;
  m_buffer |= (Mem::KVirtToPhysAddr(address) << 12);
}

constexpr const X8664::PageTableEntry& 
X8664::PageTableEntry::operator=(std::uint64_t src){
  m_buffer = src;
  return *this;
}

constexpr void
X8664::PageTableEntry::SetAttribute(bool val, PEOpt modifier) noexcept{
  if(val){
    m_buffer |= modifier;
  }
  else{
    m_buffer &= ~modifier;
  }
}

constexpr bool 
X8664::PageTableEntry::IsPresent() const noexcept{
  return m_buffer & PEOpt::Present;
}

constexpr void 
X8664::PageTableEntry::SetPresent(bool b) noexcept{
  SetAttribute(b, PEOpt::Present);
}

constexpr bool 
X8664::PageTableEntry::IsWriteable() const noexcept { 
  return m_buffer & PEOpt::Writeable; 
}

constexpr void 
X8664::PageTableEntry::SetWriteable(bool b) noexcept{
  SetAttribute(b, PEOpt::Writeable);
}

constexpr bool 
X8664::PageTableEntry::IsUserAccessible() const noexcept{ 
  return m_buffer & PEOpt::UserAccessible; 
}

constexpr void 
X8664::PageTableEntry::SetUserAccessible(bool b) noexcept{
  SetAttribute(b, PEOpt::UserAccessible);
}

constexpr bool 
X8664::PageTableEntry::IsPwt() const noexcept{ 
  return m_buffer & PEOpt::CacheWritethrough;
}

constexpr void 
X8664::PageTableEntry::SetPwt(bool b) noexcept{
  SetAttribute(b, PEOpt::CacheWritethrough);
}

constexpr bool 
X8664::PageTableEntry::IsPcd() const noexcept{ 
  return m_buffer & PEOpt::CacheDisable;
}

constexpr void 
X8664::PageTableEntry::SetPcd(bool b) noexcept{
  SetAttribute(b, PEOpt::CacheDisable);
}

constexpr bool 
X8664::PageTableEntry::IsAccessed() const noexcept{ 
  return m_buffer & PEOpt::Accessed;  
}

constexpr void 
X8664::PageTableEntry::SetAccessed(bool b) noexcept{
  SetAttribute(b, PEOpt::Accessed);
}

constexpr bool 
X8664::PageTableEntry::IsDirty() const noexcept{ 
  return m_buffer & PEOpt::Dirty;  
}

constexpr void 
X8664::PageTableEntry::SetDirty(bool b) noexcept{
  SetAttribute(b, PEOpt::Dirty);
}

constexpr bool 
X8664::PageTableEntry::IsGlobal() const noexcept{ 
  return m_buffer & PEOpt::Global;  
}

constexpr void 
X8664::PageTableEntry::SetGlobal(bool b) noexcept{
  SetAttribute(b, PEOpt::Global);
}
