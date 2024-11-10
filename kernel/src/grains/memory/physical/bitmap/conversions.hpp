#include "grains/grains.hpp"
#include "serial/kostream.hpp"
#include "include/kassert.hpp"
#include "memory/physical/bitmap.hpp"
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

} // namespace Grains::CPU
