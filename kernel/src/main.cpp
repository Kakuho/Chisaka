#include <cstdint>

#include "firmware/limine/requests.hpp"
#include "x86_64/utilites.hpp"
#include "memory/heap/tests/test.hpp"
#include "memory/heap/tests/page_size/test_run.hpp"

// Extern declarations for global constructor array.

extern void (*__init_array[])();
extern void (*__init_array_end[])();

extern "C"{
  void invokeinterrupt1();
}

extern "C" void _start() {
  // if limine is not supported, then kill the system
  if(limine::is_limine_supported()){
    X8664::HaltCatchFire();
  }

  // Call global constructors.
  for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
      __init_array[i]();
  }

  Mem::Heap::Test::Start();

  kout << "Rarity is cute pony" << '\n';
  X8664::HaltCatchFire();
}
