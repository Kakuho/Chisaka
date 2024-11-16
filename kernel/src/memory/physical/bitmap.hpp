#ifndef MEMORY_PHYS_BITMAP_HPP
#define MEMORY_PHYS_BITMAP_HPP

#define DEBUG 1

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
#include "./../memory_map/memory_map_descriptor.hpp"

#include "limine/limine.h"
#include "limine/requests.hpp"
#include "drivers/serial/kostream.hpp"
#include "lib/string.h"

namespace Mem::Phys{

class Bitmap{
  using ByteType = char;
  using AddrType = void*;

  // Regions represents continuous areas of useable available memory
  //  addresses are valid within [startAddr, endAddr]
  //  indexes maps to the bitmap indexes, within [startIndex, endIndex]
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
      return (startIndex <= index) && (index <= endIndex);
    }

    friend kostream& operator<<(kostream& kost, const Region& region){
      kout << "[ " << region.startAddr << ", "
           << region.endAddr   << " ] :: "
           << "[ " << region.startIndex << ", "
           << region.endIndex  << " ]\n";
      return kost;
    }
  };

  static_assert(sizeof(Region) == 24);

  // Handle from the stack to the bitmap data
  struct BitmapHandle{
    char* bitmap = nullptr;
    Region* regions = nullptr;
    void* end = nullptr;
  };

  public:
    // ------------------------------------------------------ //
    //  Lifetime and initial setup
    // ------------------------------------------------------ //

    explicit Bitmap() noexcept;
    explicit Bitmap(const MemoryMapDescriptor& map) noexcept;
    ~Bitmap() = default;

  private:
    void InitialiseBitmap(void* base, std::size_t length) noexcept;
    void InitialiseBitmapMemmap(void* base, std::size_t length, 
          const MemoryMapDescriptor& map) noexcept;

    void InitialiseRegions(Region* base) noexcept;
    void InitialiseRegionsMemmap(Region* base, 
        const MemoryMapDescriptor& map) noexcept;

    void Epilogue() noexcept;

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

    // ------------------------------------------------------ //
    //  Checking how much space the bitmap requires
    // ------------------------------------------------------ //

    [[nodiscard]] std::size_t RegionSizeNBytes() const noexcept;
    [[nodiscard]] std::size_t 
    RegionSizeNBytes(const MemoryMapDescriptor& map) const noexcept;

    [[nodiscard]] std::size_t PageFrameSizeNBytes() const noexcept;
    [[nodiscard]] std::size_t 
    PageFrameSizeNBytes(const MemoryMapDescriptor& map) const noexcept;

    [[nodiscard]] constexpr std::size_t TotalPageFrames() const noexcept{
      return m_maxIndex*8;
    }

    // ------------------------------------------------------ //
    //  Printing
    // ------------------------------------------------------ //

    void PrintRegions() const noexcept;
    void PrintRegions(const MemoryMapDescriptor& map) const noexcept;
    void PrintBitmap() const noexcept;

  private:

    // helpers for the pmm interface
    void ClearIndex(std::size_t index) noexcept;
    void SetIndex(std::size_t index) noexcept;
    std::uint8_t GetFreeIndex(std::uint8_t byte) const noexcept;

  private:
    BitmapHandle m_handle;
    const std::size_t m_maxIndex;           //  max pageframe index
    std::size_t m_lastIndex;                //  the last used byte index
    mutable std::size_t m_useableIndicies;  //  total byte indicies 
                                            //  which are free
    mutable std::size_t m_totalPageBytes{0};
    mutable std::size_t m_totalRegionBytes{0};
};

}

#endif
