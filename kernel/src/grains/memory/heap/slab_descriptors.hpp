#include "grains/grains.hpp"
#include "drivers/serial/kostream.hpp"
#include "kassert.hpp"
#include "memory/heap/slab/slab_descriptor.hpp"

namespace Grains::Mem{

  inline void SlabDescriptorInstantiation(){
    // PLEASE CHECK REGISTERS!
    GRAIN("Checking the bytes of the bitmap",
      {
        ::Mem::Heap::Slab::T::SlabDescriptor<int> slabDesc{reinterpret_cast<void*>(0x50000)};
        //slabDesc.Allocate();
        using bigboii = char[0x1234];
        ::Mem::Heap::Slab::T::SlabDescriptor<bigboii> bigSlab{reinterpret_cast<void*>(0x51000)};
      }
    );
  }

} // namespace Grains::Mem
