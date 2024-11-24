#include "grains/grains.hpp"
#include "drivers/serial/kostream.hpp"
#include "lib/kassert.hpp"
#include "memory/heap/slab/slab_descriptor.hpp"
#include "memory/heap/slab/cache_descriptor.hpp"

namespace Grains::Mem{

  inline void SlabDescriptorInstantiation(){
    // PLEASE CHECK REGISTERS!
    GRAIN("Checking the bytes of the bitmap",
      {
        /*
        ::Mem::Heap::Slab::T::SlabDescriptor<int> slabDesc{reinterpret_cast<void*>(0x1000)};
        slabDesc.Allocate();
        */

        ::Mem::Heap::Slab::T::CacheDescriptor<int> cacheDesc{};
        cacheDesc.AllocateSlab();
      }
    );
  }

} // namespace Grains::Mem
