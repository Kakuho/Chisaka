#include "drivers/serial/kostream.hpp"
#include "memory/features.hpp"
#include "grains/grains.hpp"
#include "lib/kassert.hpp"

namespace Grains::Mem{

  inline void PrintMemoryMap(){
    // PLEASE CHECK REGISTERS!
    GRAIN("prints the memory map",
      {
        ::Mem::PrintMemoryMap();
        kout << "Usable memory: " << ::Mem::TotalUseableMemory();
      }
    );
  }

} // namespace Grains::CPU
