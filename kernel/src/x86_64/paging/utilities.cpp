#include "utilities.hpp"

namespace X8664{

Mem::physaddr_t GetBaseTable(){
  // intel vol3a.4.5.4
  static constexpr std::uint64_t MASK = (0xFFFFFFFul << 12);
  std::uint64_t cr3 = get_cr3();
  return cr3 & MASK;
}
  
} // namespace X8664
