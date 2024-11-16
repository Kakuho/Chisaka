#ifndef KASSERT_HPP
#define KASSERT_HPP

// A non returning assert to enforce pre conditions within the kernel
//
// When compiled with -DEBUG_KASSERTS, the expression which failed
// the assetrt is printed to the screen

#include <cstdint>

#include "x86_64/utilites.hpp"
#include "drivers/serial/kostream.hpp"

// hacky way to obtain the file path and the line number of code
// for any file

#ifdef DEBUG_KASSERTS

#define kassert(expr)                                   \
  if((expr)){}                                          \
  else                                                  \
    kassert_fail_debug_impl(__func__, __FILE__, __LINE__, #expr)  \

#else

#define kassert(expr)                         \
  if((expr)){}                                \
  else                                        \
    kassert_fail_impl(__FILE__, __LINE__)     \

#endif

// the failure handlers

// debug handler contains more information, but at the cost of allocating more
// strings, which leads to increased binary size
void kassert_fail_debug_impl(
    const char* functionName, 
    const char* filename, 
    std::size_t lineNumber, 
    const char* expression
);

void kassert_fail_impl(
    const char* filename, 
    std::size_t
);

#endif
