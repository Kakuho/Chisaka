#include <cstdint>

#include "firmware/limine/requests.hpp"
#include "x86_64/utilites.hpp"
#include "grains/memory/memorymap.hpp"
#include "drivers/sata/fis/h2d_register.hpp"
#include "drivers/ahci/samples/identify_device.hpp"
#include "drivers/ahci/samples/write.hpp"

#include "memory/heap/allocator.hpp"
#include "memory/page/page_allocator.hpp"

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

  Mem::PageAllocator::Initialise();
  Mem::Heap::Allocator::Initialise();

  Mem::PrintMemoryMap();

  //Drivers::Ahci::Samples::IdentifyDevice::Try();
  Drivers::Ahci::Samples::Write::Try();

  kout << "Rarity is cute pony" << '\n';
  X8664::HaltCatchFire();
}
