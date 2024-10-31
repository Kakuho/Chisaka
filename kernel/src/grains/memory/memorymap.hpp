#include "serial/kostream.hpp"
#include "memory/updated/features.hpp"
#include "grains/grains.hpp"
#include "include/kassert.hpp"

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