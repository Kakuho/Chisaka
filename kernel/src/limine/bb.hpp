#ifndef bb_hpp
#define bb_hpp
#include <cstdint>
#include "requests.hpp"
#include "memory/memory.hpp"
#include "cpu/util.hpp"


namespace limine{

inline void initialiseFramebuffer(){
  // Ensure we got a framebuffer.
  if (limine::requests::framebuffer_request.response == nullptr
   || limine::requests::framebuffer_request.response->framebuffer_count < 1) {
    x8664::haltAndCatchFire();
  }

  // Fetch the first framebuffer.
  limine_framebuffer *framebuffer = limine::requests::framebuffer_request.response->framebuffers[0];

  // Note: we assume the framebuffer model is RGB with 32-bit pixels.
  for (std::size_t i = 0; i < 100; i++) {
      volatile std::uint32_t *fb_ptr = static_cast<volatile std::uint32_t *>(framebuffer->address);
      fb_ptr[i] = 0xff0000;
  }
}


inline constexpr memory::paddr64_t pagingTable = 0x7ff46000;

} // namespace limine

#endif
