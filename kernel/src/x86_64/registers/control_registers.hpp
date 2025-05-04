#pragma once

// Hooks to the control registers

#include <cstdint>

extern "C" std::uint64_t readcr0();

extern "C" void writecr0(std::uint32_t val);

extern "C" std::uint64_t readcr2();

extern "C" std::uint64_t get_cr3();
extern "C" void set_cr3(std::uint64_t);

extern "C" std::uint64_t readcr4();
