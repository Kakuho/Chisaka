#include "grains/grains.hpp"
#include "serial/kostream.hpp"
#include "include/kassert.hpp"
#include "memory/physical/bitmap.hpp"
#include "memory/memory_map/memory_map_descriptor.hpp"
#include "memory/address.hpp"
#include "memory/features.hpp"

namespace Grains::Mem{

  inline void BitmapRegionTests(){
    // PLEASE CHECK REGISTERS!
    GRAIN("Checking the Region array of the bitmp",
      {
        ::Mem::PrintMemoryMap();
        ::Mem::Phys::Bitmap alloc{};
        alloc.PrintRegions();
      }
    );
  }

  inline void BitmapRegion_Memmap_Tests(){
    // PLEASE CHECK REGISTERS!
    GRAIN("Checking the Region array of the bitmp",
      {
        ::Mem::MemoryMapDescriptor limineMap{};
        limineMap.Print();
        ::Mem::Phys::Bitmap alloc{limineMap};
        alloc.PrintRegions();
        REQUIRE_EQUAL(alloc.RegionSizeNBytes(limineMap), 24*limineMap.UseableEntries());
      }
    );
  }


} // namespace Grains::CPU
