#include "x86_64/features/features.hpp"
#include "grains/grains.hpp"

namespace Grains::CPU{

  inline void CpuId0_Test(){
    GRAIN("Tests to see cpuid 0",
      {
        X8664::Features::PrintManufacturerId();
      }
    );
  }
} // namespace Grains::CPU
