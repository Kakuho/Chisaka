#include <cstdint>

#include "limine/utility.hpp"
#include "x86_64/utilites.hpp"
#include "drivers/serial/kostream.hpp"

// Extern declarations for global constructor array.

extern void (*__init_array[])();
extern void (*__init_array_end[])();

extern "C"{
  void invokeinterrupt1();
}

extern "C" void _start() {
  // if limine is not supported, then kill the system
  if(Limine::Supported()){
    X8664::HaltCatchFire();
  }

  // Call global constructors.
  for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
      __init_array[i]();
  }

  kout << "Tottentemo Daisuke" << '\n';
  X8664::HaltCatchFire();
}
