#include "utility.hpp"

namespace Limine{

// Set the base revision to 1, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info

bool Supported(){
  // checks to see if the bootloader supports our standard
  LIMINE_BASE_REVISION(2);
  if(LIMINE_BASE_REVISION_SUPPORTED == false){
    kout << "LIMINE BASE 2 IS NOT SUPPORTED, ASSSUMING BASE REVISION 0" << '\n';
    return false;
  }
  else{
    return true;
  }
};

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

volatile limine_rsdp_request rsdp_request{
  .id = LIMINE_RSDP_REQUEST,
  .revision = 0,
  .response = nullptr
};

} 
