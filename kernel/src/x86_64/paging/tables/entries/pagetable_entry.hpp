#ifndef X8664_PAGING_PAGETABLE_ENTRY_HPP
#define X8664_PAGING_PAGETABLE_ENTRY_HPP

//  Only the Page Table Entry have a different shape compared to the 
//  other table entries
//
//  Reference: Intel Vol3.4.5

#include <cstdint>
#include <cassert>

#include "memory/updated/alignment.hpp"
#include "memory/updated/address.hpp"

namespace X8664::Paging{

// Helper flag modifier for page table entry attributes

namespace {
  enum PageEntryModifier: std::uint16_t{
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
}

static inline constexpr std::uint16_t P    = PageEntryModifier::Present;
static inline constexpr std::uint16_t RW   = PageEntryModifier::Writeable;
static inline constexpr std::uint16_t US   = PageEntryModifier::UserAccessible;
static inline constexpr std::uint16_t PWT  = PageEntryModifier::CacheWritethrough;
static inline constexpr std::uint16_t PCD  = PageEntryModifier::CacheDisable;
static inline constexpr std::uint16_t A    = PageEntryModifier::Accessed;
static inline constexpr std::uint16_t D    = PageEntryModifier::Dirty;
static inline constexpr std::uint16_t PAT  = PageEntryModifier::Pat;
static inline constexpr std::uint16_t G    = PageEntryModifier::Global;

class PageTableEntry{
  using VirtAddr_t = std::uint64_t;
  public:
    //-------------------------------------------------------------
    //  Initialisation
    //-------------------------------------------------------------

    constexpr explicit PageTableEntry(VirtAddr_t address, std::uint8_t flags) 
      noexcept
    {
      InitialiseBuffer(address, flags);
    }

    constexpr operator std::uint64_t() noexcept{ return m_buffer;}

    //-------------------------------------------------------------
    //  Attribute Queries
    //-------------------------------------------------------------

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

    // page table entry specific

    [[nodiscard]] constexpr bool IsDirty() const noexcept 
    { return m_buffer & Paging::A;  }

    [[nodiscard]] constexpr bool IsGlobal() const noexcept 
    { return m_buffer & Paging::A;  }

    //-------------------------------------------------------------
    //  Attribute Modifiers
    //-------------------------------------------------------------

    constexpr void SetPresent(bool b) noexcept
    {SetAttribute(b, PageEntryModifier::Present);}

    constexpr void SetWriteable(bool b) noexcept
    {SetAttribute(b, PageEntryModifier::Writeable);}

    constexpr void SetUserAccessible(bool b) noexcept
    {SetAttribute(b, PageEntryModifier::UserAccessible);}

    constexpr void SetPwt(bool b) noexcept
    {SetAttribute(b, PageEntryModifier::CacheWritethrough);}

    constexpr void SetPcd(bool b) noexcept
    {SetAttribute(b, PageEntryModifier::CacheDisable);}

    constexpr void SetAccessed(bool b) noexcept
    {SetAttribute(b, PageEntryModifier::Accessed);}

    constexpr void SetDirty(bool b) noexcept
    {SetAttribute(b, PageEntryModifier::Dirty);}

    constexpr void SetGlobal(bool b) noexcept
    {SetAttribute(b, PageEntryModifier::Global);}

  private:
    constexpr void 
    InitialiseBuffer(VirtAddr_t address, std::uint8_t flags) noexcept{
      // assumption: address is a kernel virtual address
      m_buffer = 0;
      m_buffer |= flags;
      m_buffer |= (Mem::KVirtToPhysAddr(address) << 12);
    }

    constexpr void 
    SetAttribute(bool val, PageEntryModifier modifier) noexcept{
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

static_assert(sizeof(PageTableEntry) == 8);

} // namespace X8664::Paging

#endif
