#pragma once

//  A type to represent the common entry type, since they almost all have 
//  the same data entries
//
//  Reference: Intel Vol3.4.5

#include <cstdint>
#include <cassert>

#include "memory/alignment.hpp"
#include "memory/address.hpp"

namespace X8664{

// Helper flag modifier for page table entry attributes

enum UPeOpt{
  Present = 1 << 0,
  Writeable = 1 << 1,
  UserAccessible = 1 << 2,
  CacheWritethrough = 1 << 3,
  CacheDisable = 1 << 4,
  Accessed = 1 << 5,
};

class UpperPageEntry{
  using VirtAddr_t = std::uint64_t;
  static inline constexpr std::uint64_t ADDRESS_MASK = (0xFFF'FFFFl << 12);
  public:
    constexpr explicit UpperPageEntry(): m_buffer{0}{}
    constexpr explicit UpperPageEntry(VirtAddr_t address, std::uint8_t flags) noexcept;
    constexpr explicit UpperPageEntry(std::uint64_t src) noexcept : m_buffer{src}{}

    constexpr const UpperPageEntry& operator=(std::uint64_t src);
    constexpr operator std::uint64_t() const noexcept{ return m_buffer;}
    constexpr operator std::uint64_t() noexcept{ return m_buffer;}

    constexpr std::uint64_t BaseAddress() const noexcept;

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

  private:
    constexpr void InitialiseBuffer(VirtAddr_t address, std::uint8_t flags) noexcept;
    constexpr void SetAttribute(bool val, UPeOpt modifier) noexcept;

  private:
    std::uint64_t m_buffer;
};

static_assert(sizeof(UpperPageEntry) == 8);

} // namespace X8664::Paging

// impl

constexpr X8664::UpperPageEntry::UpperPageEntry(
    VirtAddr_t address, 
    std::uint8_t flags
) noexcept
{
  InitialiseBuffer(address, flags);
}

constexpr void 
X8664::UpperPageEntry::InitialiseBuffer(VirtAddr_t address, std::uint8_t flags) noexcept{
  // assumption: address is a kernel virtual address
  m_buffer = 0;
  m_buffer |= flags;
  m_buffer |= (Mem::KVirtToPhysAddr(address) << 12);
}

constexpr const X8664::UpperPageEntry& 
X8664::UpperPageEntry::operator=( std::uint64_t src)
{
  m_buffer = src;
  return *this;
}

constexpr void 
X8664::UpperPageEntry::SetAttribute(bool val, UPeOpt modifier) noexcept{
  if(val){
    m_buffer |= modifier;
  }
  else{
    m_buffer &= ~modifier;
  }
}

constexpr std::uint64_t 
X8664::UpperPageEntry::BaseAddress() const noexcept 
{ return m_buffer & ADDRESS_MASK;  }

constexpr bool 
X8664::UpperPageEntry::IsPresent() const noexcept 
{ return m_buffer & UPeOpt::Present;  }

constexpr void 
X8664::UpperPageEntry::SetPresent(bool b) noexcept    
{SetAttribute(b, UPeOpt::Present);}

constexpr bool 
X8664::UpperPageEntry::IsWriteable() const noexcept 
{ return m_buffer & UPeOpt::Writeable; }

constexpr void 
X8664::UpperPageEntry::SetWriteable(bool b) noexcept  
{SetAttribute(b, UPeOpt::Writeable);}

constexpr bool 
X8664::UpperPageEntry::IsUserAccessible() const noexcept 
{ return m_buffer & UPeOpt::UserAccessible; }

constexpr void 
X8664::UpperPageEntry::SetUserAccessible(bool b) noexcept
{SetAttribute(b, UPeOpt::UserAccessible);}

constexpr bool 
X8664::UpperPageEntry::IsPwt() const noexcept 
{ return m_buffer & UPeOpt::CacheWritethrough;}

constexpr void 
X8664::UpperPageEntry::SetPwt(bool b) noexcept
{SetAttribute(b, UPeOpt::CacheWritethrough);}

constexpr bool 
X8664::UpperPageEntry::IsPcd() const noexcept 
{ return m_buffer & UPeOpt::CacheDisable;}

constexpr void 
X8664::UpperPageEntry::SetPcd(bool b) noexcept
{SetAttribute(b, UPeOpt::CacheDisable);}

constexpr bool 
X8664::UpperPageEntry::IsAccessed() const noexcept 
{ return m_buffer & UPeOpt::Present;  }

constexpr void 
X8664::UpperPageEntry::SetAccessed(bool b) noexcept
{SetAttribute(b, UPeOpt::Accessed);}
