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

namespace PEOpt{
  using Underlying_T = std::uint16_t;
  static inline constexpr Underlying_T Present = 1 << 0;
  static inline constexpr Underlying_T Writeable = 1 << 1;
  static inline constexpr Underlying_T UserAccessible = 1 << 2;
  static inline constexpr Underlying_T CacheWritethrough = 1 << 3;
  static inline constexpr Underlying_T CacheDisable = 1 << 4;
  static inline constexpr Underlying_T Accessed = 1 << 5;
  static inline constexpr Underlying_T Dirty = 1 << 6;
  static inline constexpr Underlying_T Pat = 1 << 7;
  static inline constexpr Underlying_T Global = 1 << 8;
}

class PageEntry{
  using VirtAddr_t = std::uint64_t;
  static inline constexpr std::uint64_t ADDRESS_MASK = (0xFFF'FFFFl << 12);

  public:
    constexpr explicit PageEntry() noexcept: m_buffer{0}{}
    constexpr explicit PageEntry(VirtAddr_t address, 
                                      std::uint8_t flags) noexcept;
    constexpr explicit PageEntry(std::uint64_t src) noexcept: m_buffer{src}{}

    constexpr const PageEntry& operator=(std::uint64_t src);
    constexpr operator std::uint64_t() noexcept{ return m_buffer;}
    constexpr operator std::uint64_t() const noexcept{ return m_buffer;}

    void PrintValues() const noexcept;

    constexpr std::uint64_t BaseAddress() const noexcept;
    constexpr void SetBaseAddress(void* address) const noexcept;

    void* PageBase() const noexcept{
      return reinterpret_cast<void*>(BaseAddress());
    }

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
    constexpr void SetAttribute(bool val, PEOpt::Underlying_T flag) noexcept;

  private:
    std::uint64_t m_buffer;
};

static_assert(sizeof(PageEntry) == 8);

} // namespace X8664::Paging

// Impl

constexpr X8664::PageEntry::PageEntry(VirtAddr_t address, 
                                                std::uint8_t flags) noexcept
{
  InitialiseBuffer(address, flags);
}

constexpr void 
X8664::PageEntry::InitialiseBuffer(VirtAddr_t address, std::uint8_t flags) noexcept{
  // assumption: address is a kernel virtual address
  m_buffer = 0;
  m_buffer |= flags;
  m_buffer |= (Mem::KVirtToPhysAddr(address) << 12);
}

constexpr const X8664::PageEntry& 
X8664::PageEntry::operator=(std::uint64_t src){
  m_buffer = src;
  return *this;
}

inline void 
X8664::PageEntry::PrintValues() const noexcept{
  kout << intmode::hex
       << "0x" << m_buffer << " ::"
       << "(Base: " << BaseAddress()
       << ", P: " << IsPresent()
       << ", W: " << IsWriteable()
       << ", U: " << IsUserAccessible()
       << ", Pwt: " << IsPwt()
       << ", Pcd: " << IsPcd()
       << ", A: " << IsAccessed()
       << ", D: " << IsDirty()
       << ", G: " << IsGlobal()
       << ')' << '\n';
}

constexpr std::uint64_t 
X8664::PageEntry::BaseAddress() const noexcept{
  return m_buffer & ADDRESS_MASK;
}

constexpr void
X8664::PageEntry::SetAttribute(bool val, PEOpt::Underlying_T flag) noexcept{
  if(val){
    m_buffer |= flag;
  }
  else{
    m_buffer &= ~flag;
  }
}

constexpr bool 
X8664::PageEntry::IsPresent() const noexcept{
  return m_buffer & PEOpt::Present;
}

constexpr void 
X8664::PageEntry::SetPresent(bool b) noexcept{
  SetAttribute(b, PEOpt::Present);
}

constexpr bool 
X8664::PageEntry::IsWriteable() const noexcept { 
  return m_buffer & PEOpt::Writeable; 
}

constexpr void 
X8664::PageEntry::SetWriteable(bool b) noexcept{
  SetAttribute(b, PEOpt::Writeable);
}

constexpr bool 
X8664::PageEntry::IsUserAccessible() const noexcept{ 
  return m_buffer & PEOpt::UserAccessible; 
}

constexpr void 
X8664::PageEntry::SetUserAccessible(bool b) noexcept{
  SetAttribute(b, PEOpt::UserAccessible);
}

constexpr bool 
X8664::PageEntry::IsPwt() const noexcept{ 
  return m_buffer & PEOpt::CacheWritethrough;
}

constexpr void 
X8664::PageEntry::SetPwt(bool b) noexcept{
  SetAttribute(b, PEOpt::CacheWritethrough);
}

constexpr bool 
X8664::PageEntry::IsPcd() const noexcept{ 
  return m_buffer & PEOpt::CacheDisable;
}

constexpr void 
X8664::PageEntry::SetPcd(bool b) noexcept{
  SetAttribute(b, PEOpt::CacheDisable);
}

constexpr bool 
X8664::PageEntry::IsAccessed() const noexcept{ 
  return m_buffer & PEOpt::Accessed;  
}

constexpr void 
X8664::PageEntry::SetAccessed(bool b) noexcept{
  SetAttribute(b, PEOpt::Accessed);
}

constexpr bool 
X8664::PageEntry::IsDirty() const noexcept{ 
  return m_buffer & PEOpt::Dirty;  
}

constexpr void 
X8664::PageEntry::SetDirty(bool b) noexcept{
  SetAttribute(b, PEOpt::Dirty);
}

constexpr bool 
X8664::PageEntry::IsGlobal() const noexcept{ 
  return m_buffer & PEOpt::Global;  
}

constexpr void 
X8664::PageEntry::SetGlobal(bool b) noexcept{
  SetAttribute(b, PEOpt::Global);
}
