#ifndef PMM_HPP
#define PMM_HPP

#include <cstdint>
#include <array>
#include <limine.h>
#include "memory.hpp"
#include <kostream.hpp>

namespace req{
  extern volatile limine_hhdm_request hhdm_request;
  extern volatile limine_memmap_request memorymap_request;
} // namespace req

namespace mem{

class pmm{
  // os shall use a bitmap for bookkeeping page frames
  std::uint8_t* bitmap;
  // bitmap is such that
  std::size_t freeram;
  std::size_t usedram;

  public:
  pmm();
  void initialiseBitmap();
};

} // namespace mem

#endif
