#include <cstdint>

#include "limine/requests.hpp"
#include "cpu/utilites.hpp"
#include "grains/cpu/features/cpuid0.hpp"

// Extern declarations for global constructor array.

extern void (*__init_array[])();
extern void (*__init_array_end[])();

// start of my own code

extern "C"{
  void invokeinterrupt1();
}

extern "C" void _start() {
  // ------------------------------------------------------ //
  //  Common Limine Init Code
  // ------------------------------------------------------ //

  // if limine is not supported, then kill the system
  if(limine::is_limine_supported()){
    X8664::HaltCatchFire();
  }

  // Call global constructors.
  for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
      __init_array[i]();
  }

  // ------------------------------------------------------ //
  //  Runtime tests for now
  // ------------------------------------------------------ //

  Grains::CPU::CpuId0_Test();
}
