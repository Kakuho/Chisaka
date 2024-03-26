#include <cstdint>
#include <array>
#include <cstddef>
#include <stdio.h>
#include <string.h>
#include <limine.h>
#include <logger.hpp>
#include <gdt.hpp>
#include <memory.hpp>
#include <kostream.hpp>
#include <limine_services.hpp>

extern "C" void outb(int, char val);
extern "C" unsigned int memory_low();

const int port = 0x3F8;
logger serialLogger{port};

namespace req{
// limine requests
extern volatile limine_framebuffer_request framebuffer_request;
extern volatile limine_memmap_request memorymap_request;
extern volatile limine_hhdm_request hhdm_request;
}

// Halt and catch fire function.
namespace {

void hcf() {
  asm ("cli");
  for (;;) {
      asm ("hlt");
  }
}

}

// The following two stubs are required by the Itanium C++ ABI (the one we use,
// regardless of the "Itanium" nomenclature).
// Like the memory functions above, these stubs can be moved to a different .cpp file,
// but should never be removed.
extern "C" {
  int __cxa_atexit(void (*)(void *), void *, void *) { return 0; }
  void __cxa_pure_virtual() { hcf(); }
}

// Extern declarations for global constructors array.
extern void (*__init_array[])();
extern void (*__init_array_end[])();

// The following will be our kernel's entry point.
// If renaming _start() to something else, make sure to change the
// linker script accordingly.
extern "C" void _start() {
  // Ensure the bootloader actually understands our base revision (see spec).
  /*
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
      hcf();
  }
  */

  // Call global constructors.
  // global ctors are just ctors for global objects
  for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
      __init_array[i]();
  }

  // Ensure we got a framebuffer.
  if (req::framebuffer_request.response == nullptr
   || req::framebuffer_request.response->framebuffer_count < 1) {
      hcf();
  }

  // Fetch the first framebuffer.
  limine_framebuffer *framebuffer = req::framebuffer_request.response->framebuffers[0];

  // Note: we assume the framebuffer model is RGB with 32-bit pixels.
  for (std::size_t i = 0; i < 100; i++) {
      volatile std::uint32_t *fb_ptr = static_cast<volatile std::uint32_t *>(framebuffer->address);
      //fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
      //fb_ptr[i * (framebuffer->pitch / 4)] = 0xff;
      fb_ptr[i] = 0xff0000;
      //kout << static_cast<std::uint32_t>(fb_ptr[i]);
  }

  //mem::printMemoryMap();
  mem::probeMemory(4);

  // We're done, just hang...
  hcf();
}
