#ifndef INTERRUPTS_CONSTANTS_HPP
#define INTERRUPTS_CONSTANTS_HPP

#include <cstdint>

namespace X8664{
  static inline constexpr std::uint8_t KERNEL_CODE_SEGMENT{0b1000};
  static inline constexpr std::uint8_t CALL_GATE_TYPE{0b1110};
}

#endif
