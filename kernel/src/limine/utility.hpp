#pragma once

#include "limine.h"
#include "drivers/serial/kostream.hpp"

namespace Limine{

bool Supported();

extern volatile limine_framebuffer_request framebuffer_request;

extern volatile limine_memmap_request memorymap_request;

extern volatile limine_hhdm_request hhdm_request;

extern volatile limine_kernel_address_request kernel_addr_req;

extern volatile limine_dtb_request dtb_request;

extern volatile limine_rsdp_request rsdp_request;

}
