#ifndef MEMORY_PHYS_BITMAP_HPP
#define MEMORY_PHYS_BITMAP_HPP

// Bitmap Page Frame Allocator

#include <cstdint>

#include "bump.hpp"
#include "limine/requests.hpp"

namespace Mem::Phys{

struct BitmapHandle{
  using AddrType = void*;
  AddrType bitmap;
  AddrType regions;
};

class Bitmap{
  using ByteType = char;
  using AddrType = void*;

  struct Regions{
    std::uint64_t startAddr;
    std::uint64_t endAddr;
    std::uint32_t startIndex;
    std::uint32_t endIndex;
  };

  static_assert(sizeof(Regions) == 24);

  public:
    // ------------------------------------------------------ //
    //  Lifetime and initial setup
    // ------------------------------------------------------ //

    explicit Bitmap() noexcept;

  private:
    void InitialiseBitmap(AddrType bitmapBase);
    void InitialiseRegions(AddrType regionBase);

  public:
    // ------------------------------------------------------ //
    //  Allocation and Deallocation
    // ------------------------------------------------------ //

    AddrType AllocatePage();
    void FreePage();

  private:
    BitmapHandle m_handle;
};

}

#endif
