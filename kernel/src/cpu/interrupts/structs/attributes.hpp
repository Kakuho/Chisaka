#ifndef INTERRUPTS_ATTRIBUTES_HPP
#define INTERRUPTS_ATTRIBUTES_HPP

// useful type safe argument packed structs for constructors. 

#include <cstdint>

namespace X8664::InterruptAttributes{

  struct GateAttributes{
    std::uint64_t offset;
    std::uint16_t segmentSelector;
    bool isPresent;
  };

}

#endif


