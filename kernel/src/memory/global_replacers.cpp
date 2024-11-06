// this file contains global replacement functions for the usual allocation and 
// deallocation functions.

#include <cstdint>

#include "serial/kostream.hpp"
#include "x86_64/utilites.hpp"

// I won't be using exceptions to build this os

[[nodiscard]] void* operator new(std::size_t n) noexcept{
  kout << "operator new(std::size_t) is not implemented" << '\n';
  X8664::HaltCatchFire();

}

void operator delete(void* vaddr) noexcept{
  kout << "operator delete(void*) is not implemented" << '\n';
  X8664::HaltCatchFire();
}

void operator delete(void* vaddr, std::size_t size) noexcept{
  kout << "operator new(void*, std::size_t) is not implemented" << '\n';
  X8664::HaltCatchFire();
}
