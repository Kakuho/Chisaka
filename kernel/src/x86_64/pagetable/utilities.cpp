#include "utilities.hpp"

namespace X8664::Paging{

Mem::physaddr_t PagingTableBase(){
  // intel vol3a.4.5.4
  static constexpr std::uint64_t MASK = (0xFFFFFFF << 12);
  std::uint64_t cr3 = readcr3();
  return (cr3 & MASK) >> 12;
}
  
} // namespace X8664::Paging
