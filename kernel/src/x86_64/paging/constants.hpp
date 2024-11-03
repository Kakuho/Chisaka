#ifndef X8664_PAGETABLE_UTILITIES_HPP
#define X8664_PAGETABLE_UTILITIES_HPP

// Constants relating to x86_64 page tables
//
// Reference: Intel-vol3.4

#include <cstdint>

namespace X8664::Paging{
  static inline constexpr std::uint16_t PAGING_ENTRIES = 512;
}

#endif
