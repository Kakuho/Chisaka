#ifndef SERVICES_HPP
#define SERVICES_HPP

#include "limine.h"

namespace req{

// Set the base revision to 1, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info

bool is_limine_supported();

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once.

extern volatile limine_framebuffer_request framebuffer_request;

extern volatile limine_memmap_request memorymap_request;

extern volatile limine_hhdm_request hhdm_request;

extern volatile limine_kernel_address_request kernel_addr_req;

} // namespace req

#endif
