#pragma once

//  A type to represent the common entry type, since they almost all have 
//  the same data entries
//
//  Reference: Intel Vol3.4.5

#include <cstdint>
#include <cassert>

#include "memory/alignment.hpp"
#include "memory/address.hpp"
#include "drivers/serial/kostream.hpp"

namespace X8664{

class UpperPageTable;

// Helper flag modifier for page table entry attributes

namespace UPeOpt{
  using Underlying_T = std::uint8_t;
  static inline constexpr Underlying_T Present = 1 << 0;
  static inline constexpr Underlying_T Writeable = 1 << 1;
  static inline constexpr Underlying_T UserAccessible = 1 << 2;
  static inline constexpr Underlying_T CacheWritethrough = 1 << 3;
  static inline constexpr Underlying_T CacheDisable = 1 << 4;
  static inline constexpr Underlying_T Accessed = 1 << 5;
}

class UpperPageEntry{
  using VirtAddr_t = std::uint64_t;
  static inline constexpr std::uint64_t ADDRESS_MASK = (0xFFF'FFFFl << 12);
  public:
    constexpr explicit UpperPageEntry(): m_buffer{0}{}
    constexpr explicit UpperPageEntry(VirtAddr_t address, std::uint8_t flags) noexcept;
    constexpr explicit UpperPageEntry(void* address, std::uint8_t flags) noexcept:
      UpperPageEntry(reinterpret_cast<std::uint64_t>(address), flags){};

    constexpr explicit UpperPageEntry(std::uint64_t src) noexcept : m_buffer{src}{}

    constexpr const UpperPageEntry& operator=(std::uint64_t src);
    constexpr operator std::uint64_t() const noexcept{ return m_buffer;}
    constexpr operator std::uint64_t() noexcept{ return m_buffer;}

    void PrintValues() noexcept;

    constexpr std::uint64_t BaseAddress() const noexcept;

    void* TablePtr() const noexcept{
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

  private:
    constexpr void InitialiseBuffer(VirtAddr_t address, std::uint8_t flags) noexcept;
    constexpr void SetAttribute(bool val, UPeOpt::Underlying_T flag) noexcept;

  private:
    std::uint64_t m_buffer;
};

static_assert(sizeof(UpperPageEntry) == 8);

} // namespace X8664

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

inline void X8664::UpperPageEntry::PrintValues() noexcept{
  kout << intmode::hex
       << "0x" << m_buffer << " ::"
       << "(Base: " << BaseAddress() 
       << ", P: " << IsPresent() 
       << ", W: " << IsWriteable() 
       << ", U: " << IsUserAccessible()
       << ", Pwt: " << IsPwt() 
       << ", Pcd: " << IsPcd()
       << ", A: " << IsAccessed()
       << ')' << '\n';
}

constexpr std::uint64_t 
X8664::UpperPageEntry::BaseAddress() const noexcept 
{ return m_buffer & ADDRESS_MASK;  }

constexpr void 
X8664::UpperPageEntry::SetAttribute(bool val, UPeOpt::Underlying_T modifier) noexcept{
  if(val){
    m_buffer |= modifier;
  }
  else{
    m_buffer &= ~modifier;
  }
}

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
