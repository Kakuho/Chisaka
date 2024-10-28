#include "cpu/features/cpuid.hpp"
#include "grains/grains.hpp"

namespace Grains::CPU{

  inline void CpuId0_Test(){
    GRAIN("Tests to see cpuid 0",
      {
        X8664::CpuId::PrintManufacturerId();
      }
    );
  }
} // namespace Grains::CPU
