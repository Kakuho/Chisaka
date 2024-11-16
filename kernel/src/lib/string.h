#ifndef STRING_H
#define STRING_H

#include <cstdint>
#include "drivers/serial/kostream.hpp"

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .cpp file.

extern "C" {

// memcpy copies n bytes from src to dest
void *memcpy(void *dest, const void *src, std::size_t n);

// memset copies static_cast<unsigned char>(c) to first n bytes of s
void *memset(void *s, int c, std::size_t n);

// sorta the same as memcpy
void *memmove(void *dest, const void *src, std::size_t n);

// compares the first n bytes of s1 and s2
int memcmp(const void *s1, const void *s2, std::size_t n);

}

#endif
