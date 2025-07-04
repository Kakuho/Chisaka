#ifndef CPU_CPUID_HPP
#define CPU_CPUID_HPP

// handles to the assembly cpuid procedures

#include <cstdint>

extern "C" std::uint64_t cpuid_0();

extern "C" std::uint64_t cpuid_1();

extern "C" std::uint64_t cpuid_80000008();
extern "C" std::uint64_t cpuid_80000008_eax();
extern "C" std::uint64_t cpuid_80000008_ebx();

#endif
