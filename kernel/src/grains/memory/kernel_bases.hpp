#include "serial/kostream.hpp"
#include "memory/updated/address.hpp"
#include "memory/updated/features.hpp"
#include "grains/grains.hpp"
#include "include/kassert.hpp"

namespace Grains::Mem{

  inline void PrintKernelBases(){
    GRAIN("prints the base addresses of the kernel",
      {
        ::Mem::PrintMemoryMap();

        kout  << intmode::hex
              << "Kernel Base Virtual: " 
              << ::Mem::KernelVirtBase() 
              << '\n'
              << "Kernel Base Physical: " 
              << ::Mem::KernelPhysBase()
              << '\n';

        int CanYouSeeMe = 0x6969;
        kout << CanYouSeeMe << '\n';
      }
    );
  }

} // namespace Grains::CPU
