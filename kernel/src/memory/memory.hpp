#ifndef MEMORY_HPP
#define MEMORY_HPP
#include <cstdint>
#include <limine.h>
#include <limine_services.hpp>
#include <kostream.hpp>

namespace req{
extern volatile limine_hhdm_request hhdm_request;
extern volatile limine_memmap_request memorymap_request;
extern volatile limine_kernel_address_request kernel_addr_req;
}

namespace mem{
  // a bunch of miscellenaeous memory
  using paddr64_t = std::uint64_t;
  using vaddr64_t = std::uint64_t;

  inline paddr64_t vaddrToPaddr(vaddr64_t vaddr){
    return vaddr - req::hhdm_request.response->offset;
  }

  inline vaddr64_t paddrToVaddr(paddr64_t paddr){
    return paddr + req::hhdm_request.response->offset;
  }

  void printMemoryMap();

  void probeMemory(std::size_t index);

  std::uint64_t calculateAvailableMemory();

  void printPageFrames();

  vaddr64_t getKernelVirtualAddress();

  void upperLimitProbe();

  void lowerLimitProbe();

  /* DO NOT USE */
  void corruptMemory();
  /* DO NOT USE */

} // namespace mem

#endif
