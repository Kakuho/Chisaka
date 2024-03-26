#ifndef SERVICES_HPP
#define SERVICES_HPP

#include <limine.h>

namespace req{

// Set the base revision to 1, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info

volatile LIMINE_BASE_REVISION(1);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once.

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

volatile limine_hhdm_request hhdm_request{
  .id = LIMINE_HHDM_REQUEST,
  .revision = 0,
  .response = nullptr
};

volatile limine_kernel_address_request kernel_addr_req{
  .id = LIMINE_KERNEL_ADDRESS_REQUEST,
  .revision = 0,
  .response = nullptr
};

} // namespace requests

#endif
