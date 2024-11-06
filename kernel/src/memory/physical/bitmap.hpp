#ifndef MEMORY_PHYS_BITMAP_HPP
#define MEMORY_PHYS_BITMAP_HPP

// Bitmap Page Frame Allocator
//
// Throughout the code I use the term pageIndex to refer to an indiviual 
// pageframe's index and byteIndex to refer to the unit of pageframes
//
// Thus, in this model, a single byteIndex refers to 8 pageIndicies

#include <cstdint>

#include "bump.hpp"
#include "./../address.hpp"
#include "./../features.hpp"
#include "limine/limine.h"
#include "limine/requests.hpp"
#include "string.h"

namespace Mem::Phys{

class Bitmap{
  using ByteType = char;
  using AddrType = void*;

  private:
    // Private Data Structures useful for implementing the Bitmap allocator
    struct Region{
      std::uint64_t startAddr;
      std::uint64_t endAddr;
      std::uint32_t startIndex;
      std::uint32_t endIndex;

      [[nodiscard]] constexpr bool 
      ContainsAddr(std::uint64_t paddr) const noexcept{
        return (startAddr <= paddr) && (paddr <= endAddr);
      }

      [[nodiscard]] constexpr bool 
      ContainsAddr(void* paddr) const noexcept{
        return ContainsAddr(reinterpret_cast<std::uint64_t>(paddr));
      }

      [[nodiscard]] constexpr bool 
      ContainsIndex(std::uint32_t index) const noexcept{
        return (startAddr <= index) && (index <= endIndex);
      }
    };

    static_assert(sizeof(Region) == 24);

    struct BitmapHandle{
      char* bitmap = nullptr;
      Region* regions = nullptr;
    };

  public:
    // ------------------------------------------------------ //
    //  Lifetime and initial setup
    // ------------------------------------------------------ //

    explicit Bitmap() noexcept;
    ~Bitmap() = default;

  private:
    // helpful functions for setting up the bitmap
    [[nodiscard]] std::uint8_t RegionSizeRequired() const noexcept;
    void InitialiseRegions() noexcept;

    [[nodiscard]] std::uint8_t PageFrameSizeRequired() const noexcept;
    void InitialiseBitmap() noexcept;

  public:
    // ------------------------------------------------------ //
    //  Mapping between Physical To Index
    //    (Indexes are the individual page frame index)
    // ------------------------------------------------------ //

    [[nodiscard]] std::size_t IndexFrom(AddrType paddr) const noexcept;
    [[nodiscard]] AddrType AddressFrom(std::size_t index) const noexcept;

    // ------------------------------------------------------ //
    //  PMM Interface
    // ------------------------------------------------------ //

    [[nodiscard]] AddrType AllocPage() noexcept;
    void FreePage(AddrType paddr) noexcept;

  private:
    // helpers for the pmm interface
    void ClearIndex(std::size_t index) noexcept;
    void SetIndex(std::size_t index) noexcept;
    std::uint8_t GetFreeIndex(std::uint8_t byte) const noexcept;


  private:
    BitmapHandle m_handle;
    const std::size_t m_maxIndex;           //  max pageframe index
    std::size_t m_lastIndex;                //  the last used byte index
    mutable std::size_t m_useableIndicies;  //  total byte indicies which are free
    mutable std::size_t m_totalRegionSize;
};

}

#endif
