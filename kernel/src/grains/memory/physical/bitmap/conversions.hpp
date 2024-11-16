#include "grains/grains.hpp"
#include "memory/memory_map/memory_map_entry.hpp"
#include "drivers/serial/kostream.hpp"
#include "lib/kassert.hpp"
#include "memory/physical/bitmap.hpp"
#include "memory/memory_map/memory_map_descriptor.hpp"
#include "memory/address.hpp"
#include "memory/features.hpp"

namespace Grains::Mem{

  inline void Bitmap_Conversion_Tests(){
    // PLEASE CHECK REGISTERS!
    GRAIN("Checking the conversions",
      {
        using enum ::Mem::MemoryMapEntry::Type;

        ::Mem::MemoryMapDescriptor limineMap{};
        limineMap.Print();

        kout << "------------\n";

        ::Mem::MemoryMapDescriptor map{
          ::Mem::MemoryMapEntry{0x50000, 0x2000, Useable},
          ::Mem::MemoryMapEntry{0x52000, 0x3000, Reserved},
          ::Mem::MemoryMapEntry{0x53000, 0x1000, Useable},
        };

        map.Print();

        ::Mem::Phys::Bitmap bmAlloc{map};

        bmAlloc.PrintRegions();

        // Convert Bitmap Index to Physical Address
        
        REQUIRE_EQUAL(reinterpret_cast<std::uint64_t>(bmAlloc.AddressFrom(0)), 0x50000);
        REQUIRE_EQUAL(reinterpret_cast<std::uint64_t>(bmAlloc.AddressFrom(1)), 0x51000);
        REQUIRE_EQUAL(reinterpret_cast<std::uint64_t>(bmAlloc.AddressFrom(2)), 0x53000);

        // Convert Physical Address to Page Index

        REQUIRE_EQUAL(bmAlloc.IndexFrom(reinterpret_cast<void*>(0x50000)), 0);
        REQUIRE_EQUAL(bmAlloc.IndexFrom(reinterpret_cast<void*>(0x5005F)), 0);
        REQUIRE_EQUAL(bmAlloc.IndexFrom(reinterpret_cast<void*>(0x5089F)), 0);

        REQUIRE_EQUAL(bmAlloc.IndexFrom(reinterpret_cast<void*>(0x53000)), 2);
        REQUIRE_EQUAL(bmAlloc.IndexFrom(reinterpret_cast<void*>(0x53FFF)), 2);

        // testing allocation

        kout << "printing bitmap: ";
        bmAlloc.PrintBitmap();

        void* allocd = bmAlloc.AllocPage();

        bmAlloc.AllocPage();

        kout << "printing bitmap: ";
        bmAlloc.PrintBitmap();

        REQUIRE_EQUAL(reinterpret_cast<std::uint64_t>(allocd), 0x50000);
      }
    );
  }

} // namespace Grains::CPU
