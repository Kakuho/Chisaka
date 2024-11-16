#include "grains/grains.hpp"
#include "drivers/serial/kostream.hpp"
#include "lib/kassert.hpp"
#include "memory/memory_map/memory_map_entry.hpp"
#include "memory/memory_map/memory_map_descriptor.hpp"
#include "memory/features.hpp"

namespace Grains::Mem{

  inline void MemoryMapCustomInit1(){
    GRAIN("Checking the initialisation of custom Memory Map",
      {
        using enum ::Mem::MemoryMapEntry::Type;
        ::Mem::MemoryMapDescriptor map
        {
          ::Mem::MemoryMapEntry{0, 0x1000, Useable},
          ::Mem::MemoryMapEntry{0x1000, 0x1000, Reserved},
          ::Mem::MemoryMapEntry{0x2000, 0x1000, Useable}
        };

        // Queries

        REQUIRE_EQUAL(map.TotalUseableMemory(), 0x2000);
        REQUIRE_EQUAL(map.TotalUseablePageFrames<0x1000>(), 2);
        REQUIRE_EQUAL(map.HighestUseableAddress(), 0x2fff);
        REQUIRE_EQUAL(map.LowestUseableAddress(), 0);
        REQUIRE_EQUAL(map.LongestUseableBase(), 0);

        map.Print();
      }
    );
  }

  inline void MemoryMap_DefaultCtor(){
    GRAIN("Checking default ctor == information from limine",
      {
        using enum ::Mem::MemoryMapEntry::Type;
        ::Mem::MemoryMapDescriptor map{};

        map.Print();
        kout << "---------\n";
        ::Mem::PrintMemoryMap();
      }
    );
  }

} // namespace Grains::CPU
