#include "requests.hpp"
#include "limine.h"

namespace limine::requests{

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

volatile limine_dtb_request dtb_request{
  .id = LIMINE_DTB_REQUEST,
  .revision = 0,
  .response = nullptr
};


} // namespace limine::requests
