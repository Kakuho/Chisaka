#ifndef X8664_PAGING_COMMON_ENTRY_HPP
#define X8664_PAGING_COMMON_ENTRY_HPP

//  A type to represent the common entry type, since they almost all have 
//  the same data entries
//
//  Reference: Intel Vol3.4.5

#include <cstdint>
#include <cassert>

#include "memory/alignment.hpp"
#include "memory/address.hpp"

namespace X8664::Paging{

// Helper flag modifier for page table entry attributes

namespace {
  enum EntryModifier: std::uint8_t{
    Present = 1 << 0,
    Writeable = 1 << 1,
    UserAccessible = 1 << 2,
    CacheWritethrough = 1 << 3,
    CacheDisable = 1 << 4,
    Accessed = 1 << 5,
  };
}

static inline constexpr std::uint8_t P    = EntryModifier::Present;
static inline constexpr std::uint8_t RW   = EntryModifier::Writeable;
static inline constexpr std::uint8_t US   = EntryModifier::UserAccessible;
static inline constexpr std::uint8_t PWT  = EntryModifier::CacheWritethrough;
static inline constexpr std::uint8_t PCD  = EntryModifier::CacheDisable;
static inline constexpr std::uint8_t A    = EntryModifier::Accessed;

class CommonEntry{
  using VirtAddr_t = std::uint64_t;
  static inline constexpr std::uint64_t ADDRESS_MASK = (0xFFF'FFFF << 12);
  public:
    //-------------------------------------------------------------
    //  Initialisation
    //-------------------------------------------------------------

    constexpr explicit CommonEntry(): m_buffer{0}{}

    constexpr explicit CommonEntry(
        VirtAddr_t address, 
        std::uint8_t flags
    ) noexcept
    {
      InitialiseBuffer(address, flags);
    }

    constexpr const CommonEntry& operator=( std::uint64_t src)
    {
      m_buffer = src;
      return *this;
    }

    constexpr operator std::uint64_t() noexcept{ return m_buffer;}
    constexpr operator std::uint64_t() const noexcept{ return m_buffer;}

    //-------------------------------------------------------------
    //  Attribute Queries
    //-------------------------------------------------------------

    [[nodiscard]] constexpr std::uint64_t BaseAddress() const noexcept 
    { return m_buffer & ADDRESS_MASK;  }

    [[nodiscard]] constexpr bool IsPresent() const noexcept 
    { return m_buffer & Paging::P;  }

    [[nodiscard]] constexpr bool IsWriteable() const noexcept 
    { return m_buffer & Paging::RW; }

    [[nodiscard]] constexpr bool IsUserAccessible() const noexcept 
    { return m_buffer & Paging::US; }

    [[nodiscard]] constexpr bool IsPwt() const noexcept 
    { return m_buffer & Paging::PWT;}

    [[nodiscard]] constexpr bool IsPcd() const noexcept 
    { return m_buffer & Paging::PCD;}

    [[nodiscard]] constexpr bool IsAccessed() const noexcept 
    { return m_buffer & Paging::A;  }

    //-------------------------------------------------------------
    //  Attribute Modifiers
    //-------------------------------------------------------------

    constexpr void SetPresent(bool b) noexcept
    {SetAttribute(b, EntryModifier::Present);}

    constexpr void SetWriteable(bool b) noexcept
    {SetAttribute(b, EntryModifier::Writeable);}

    constexpr void SetUserAccessible(bool b) noexcept
    {SetAttribute(b, EntryModifier::UserAccessible);}

    constexpr void SetPwt(bool b) noexcept
    {SetAttribute(b, EntryModifier::CacheWritethrough);}

    constexpr void SetPcd(bool b) noexcept
    {SetAttribute(b, EntryModifier::CacheDisable);}

    constexpr void SetAccessed(bool b) noexcept
    {SetAttribute(b, EntryModifier::Accessed);}

  private:
    constexpr void 
    InitialiseBuffer(VirtAddr_t address, std::uint8_t flags) noexcept{
      // assumption: address is a kernel virtual address
      m_buffer = 0;
      m_buffer |= flags;
      m_buffer |= (Mem::KVirtToPhysAddr(address) << 12);
    }

    constexpr void 
    SetAttribute(bool val, EntryModifier modifier) noexcept{
      if(val){
        m_buffer |= modifier;
      }
      else{
        m_buffer &= ~modifier;
      }
    }

  private:
    std::uint64_t m_buffer;
};

static_assert(sizeof(CommonEntry) == 8);

} // namespace X8664::Paging

#endif
