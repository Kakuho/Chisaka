#include "grains/grains.hpp"
#include "drivers/serial/kostream.hpp"
#include "kassert.hpp"
#include "memory/physical/bitmap.hpp"
#include "memory/memory_map/memory_map_descriptor.hpp"
#include "memory/address.hpp"
#include "memory/features.hpp"

namespace Grains::Mem{

  inline void BitmapBytesChecking(){
    // PLEASE CHECK REGISTERS!
    GRAIN("Checking the bytes of the bitmap",
      {
        ::Mem::PrintMemoryMap();
        ::Mem::Phys::Bitmap alloc{};
        //REQUIRE_EQUAL(::Mem::TotalUseablePageFrames<0x1000>(), alloc.TotalPageFrames());
      }
    );
  }

  inline void Bitmap_Memmap_BytesChecking(){
    // PLEASE CHECK REGISTERS!
    GRAIN("Checking the bytes of the bitmap, given a memory mapping",
      {
        ::Mem::MemoryMapDescriptor limineMap{};
        limineMap.Print();
        ::Mem::Phys::Bitmap alloc{limineMap};
        REQUIRE(
            "Total page frames should be less than equals the total amount of page frames", 
            alloc.TotalPageFrames() <= limineMap.TotalUseablePageFrames<0x1000>()
        );
        std::size_t bound = 10;
        REQUIRE(
            "Total page frames should be greater than equals a lower bound", 
            alloc.TotalPageFrames() >= (limineMap.TotalUseablePageFrames<0x1000>() - bound)
        );
      }
    );
  }

} // namespace Grains::CPU
