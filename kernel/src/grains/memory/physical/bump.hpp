#include "grains/grains.hpp"
#include "drivers/serial/kostream.hpp"
#include "kassert.hpp"
#include "memory/physical/bump.hpp"
#include "memory/address.hpp"

namespace Grains::Mem{

  inline void BumpAllocator(){
    // PLEASE CHECK REGISTERS!
    GRAIN("Bump Allocation Tests",
      {
        ::Mem::Phys::Bump alloc{0x50000, 0x50000+0x2000};
        REQUIRE_EQUAL(alloc.Start(), 0x50000);
        REQUIRE_EQUAL(alloc.End(), 0x52000);

        ::Mem::physaddr_t k = alloc.AllocatePage();
        REQUIRE_EQUAL(k, 0x50000);

        k = alloc.AllocatePage();
        REQUIRE_EQUAL(k, 0x51000);

        k = alloc.AllocatePage();
        REQUIRE_EQUAL(k, 0x52000);

        k = alloc.AllocatePage();
        REQUIRE_EQUAL(k, 0x52000);
      }
    );
  }

} // namespace Grains::CPU
