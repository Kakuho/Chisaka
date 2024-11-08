#include "grains/grains.hpp"
#include "serial/kostream.hpp"
#include "include/kassert.hpp"
#include "memory/physical/bitmap.hpp"
#include "memory/address.hpp"
#include "memory/features.hpp"

namespace Grains::Mem{

  inline void BitmapAllocatorTests(){
    // PLEASE CHECK REGISTERS!
    GRAIN("Bitmap Allocation Tests",
      {
        ::Mem::PrintMemoryMap();
        ::Mem::Phys::Bitmap alloc{};
        kout << "region size required :: " << alloc.RegionSizeRequired() << '\n';
        kout << "page size required :: " << alloc.PageFrameSizeRequired() << '\n';
        //REQUIRE_EQUAL(::Mem::TotalUseablePageFrames<0x1000>(), alloc.TotalPageFrames());
        REQUIRE_EQUAL(false, true);
      }
    );
  }

} // namespace Grains::CPU
