#ifndef CPU_CONTROL_REGISTERS_HPP
#define CPU_CONTROL_REGISTERS_HPP

// Hooks to get the value of control registers

#include <cstdint>

extern "C" std::uint64_t readcr0();

extern "C" void writecr0(std::uint32_t val);

extern "C" std::uint64_t readcr2();

extern "C" std::uint64_t readcr3();

extern "C" std::uint64_t readcr4();

#endif
