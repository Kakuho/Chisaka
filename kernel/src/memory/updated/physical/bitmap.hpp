#ifndef MEMORY_PHYS_BITMAP_HPP
#define MEMORY_PHYS_BITMAP_HPP

// Bitmap Page Frame Allocator

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

  struct Region{
    std::uint64_t startAddr;
    std::uint64_t endAddr;
    std::uint32_t startIndex;
    std::uint32_t endIndex;

    bool ContainsAddr(std::uint64_t paddr) const noexcept{
      return (startAddr <= paddr) && (paddr <= endAddr);
    }

    bool ContainsAddr(void* paddr) const noexcept{
      return ContainsAddr(reinterpret_cast<std::uint64_t>(paddr));
    }

    bool ContainsIndex(std::uint32_t index) const noexcept{
      return (startAddr <= index) && (index <= endIndex);
    }
  };

  struct BitmapHandle{
    char* bitmap = nullptr;
    Region* regions = nullptr;
  };

  static_assert(sizeof(Region) == 24);

  public:
    // ------------------------------------------------------ //
    //  Lifetime and initial setup
    // ------------------------------------------------------ //

    explicit Bitmap() noexcept;

  private:
    std::uint8_t RegionSizeRequired() const noexcept;
    void InitialiseRegions() noexcept;

    std::uint8_t PageFrameSizeRequired() const noexcept;
    void InitialiseBitmap() noexcept;

  public:
    // ------------------------------------------------------ //
    //  Mapping between Physical To Index
    // ------------------------------------------------------ //


    std::size_t IndexFrom(AddrType paddr);
    AddrType AddressFrom(std::size_t index);

    // ------------------------------------------------------ //
    //  Allocation and Deallocation
    // ------------------------------------------------------ //

    AddrType AllocatePage();
    void FreePage();

  private:
    BitmapHandle m_handle;
    mutable std::size_t m_totalRegionSize;
};

}

#endif
