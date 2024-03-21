#include <cstdint>
#include <array>
#include <cstddef>
#include <stdio.h>
#include <limine.h>
#include <logger.hpp>
#include <kostream.hpp>

extern "C" void outb(int, char val);
extern "C" unsigned int memory_low();

const int port = 0x3F8;
logger serialLogger{port};

// Set the base revision to 1, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

namespace { volatile LIMINE_BASE_REVISION(1); }


// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once.

namespace {

volatile limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = nullptr
};

volatile limine_memmap_request memorymap_request{
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
    .response = nullptr
};

}

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .cpp file.

extern "C" {

// memcpy copies n bytes from src to dest
void *memcpy(void *dest, const void *src, std::size_t n) {
    std::uint8_t *pdest = static_cast<std::uint8_t *>(dest);
    const std::uint8_t *psrc = static_cast<const std::uint8_t *>(src);

    for (std::size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

// memset copies static_cast<unsigned char>(c) to first n bytes of s
void *memset(void *s, int c, std::size_t n) {
    std::uint8_t *p = static_cast<std::uint8_t *>(s);

    for (std::size_t i = 0; i < n; i++) {
        p[i] = static_cast<uint8_t>(c);
    }

    return s;
}

// sorta the same as memcpy
void *memmove(void *dest, const void *src, std::size_t n) {
    std::uint8_t *pdest = static_cast<std::uint8_t *>(dest);
    const std::uint8_t *psrc = static_cast<const std::uint8_t *>(src);

    if (src > dest) {
        for (std::size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (std::size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

// compares the first n bytes of s1 and s2
int memcmp(const void *s1, const void *s2, std::size_t n) {
    const std::uint8_t *p1 = static_cast<const std::uint8_t *>(s1);
    const std::uint8_t *p2 = static_cast<const std::uint8_t *>(s2);

    for (std::size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

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

void printMemoryMap(){
  if(memorymap_request.response == nullptr){
    serialLogger.log('M');
    serialLogger.log('E');
    serialLogger.log('M');
    serialLogger.log(' ');
    serialLogger.log('F');
    serialLogger.log('A');
    serialLogger.log('I');
    serialLogger.log('L');
    hcf();
  }

  // fetch the memory map
  serialLogger.log(memorymap_request.response->entry_count);
  std::uint64_t mem_entries_t = memorymap_request.response->entry_count;
  for(auto i{0}; i < mem_entries_t; i++){
    serialLogger.log(memorymap_request.response->entries[i]->base);
    serialLogger.log(' ');
    serialLogger.log(memorymap_request.response->entries[i]->length);
    serialLogger.log(' ');
    serialLogger.log(memorymap_request.response->entries[i]->type);
    serialLogger.log('\r');
    serialLogger.log('\n');
  }
}

void* operator new(std::size_t sz){
  serialLogger.log('F');
  return (void*)0;
}

// The following will be our kernel's entry point.
// If renaming _start() to something else, make sure to change the
// linker script accordingly.
extern "C" void _start() {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    // Call global constructors.
    // global ctors are just ctors for global objects
    for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
        __init_array[i]();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == nullptr
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    for (std::size_t i = 0; i < 100; i++) {
        volatile std::uint32_t *fb_ptr = static_cast<volatile std::uint32_t *>(framebuffer->address);
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    }

    printMemoryMap();

    serialLogger.log("ayo\n");
    kout << 1 << ' ' << 10 << "ayo am i work?" << '\n' << "ye boii i am work\n";

    // We're done, just hang...
    hcf();
}
