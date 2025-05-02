#pragma once

#include <cstdint>

namespace Aii{
  constexpr void* Memcpy(void* dest, const void* src, std::size_t n);
  constexpr void* Memset(void* s, int c, std::size_t n);
  constexpr void* Memmove(void* dest, const void* src, std::size_t n);
  constexpr int Memcmp(const void* s1, const void* s2, std::size_t n);
}

constexpr void* Aii::Memcpy(void* dest, const void* src, std::size_t n){
  std::uint8_t *pdest = static_cast<std::uint8_t *>(dest);
  const std::uint8_t *psrc = static_cast<const std::uint8_t *>(src);
  for (std::size_t i = 0; i < n; i++) {
      pdest[i] = psrc[i];
  }
  return dest;
}

constexpr void* Aii::Memset(void* s, int c, std::size_t n){
  std::uint8_t *p = static_cast<std::uint8_t *>(s);
  for (std::size_t i = 0; i < n; i++) {
      p[i] = static_cast<uint8_t>(c);
  }
  return s;
}

constexpr void* Aii::Memmove(void* dest, const void* src, std::size_t n){
  std::uint8_t *pdest = static_cast<std::uint8_t *>(dest);
  const std::uint8_t *psrc = static_cast<const std::uint8_t *>(src);
  if (src > dest) {
      for (std::size_t i = 0; i < n; i++) {
          pdest[i] = psrc[i];
      }
  } else if (src < dest) {
      for (std::size_t i = n; i > 0; i--) {
          pdest[i-1] = psrc[i-1];
      }
  }
  return dest;
}

constexpr int Aii::Memcmp(const void* s1, const void* s2, std::size_t n){
  const std::uint8_t *p1 = static_cast<const std::uint8_t *>(s1);
  const std::uint8_t *p2 = static_cast<const std::uint8_t *>(s2);
  for (std::size_t i = 0; i < n; i++) {
      if (p1[i] != p2[i]) {
          return p1[i] < p2[i] ? -1 : 1;
      }
  }
  return 0;
}

// required for GCC 

extern "C" {

inline void* memcpy(void* dest, const void* src, std::size_t n){
  return Aii::Memcpy(dest, src, n);
}

inline void* memset(void* s, int c, std::size_t n){
  return Aii::Memset(s, c, n);
}

inline void* memmove(void* dest, const void* src, std::size_t n){
  return Aii::Memmove(dest, src, n);
}

inline int memcmp(const void* s1, const void* s2, std::size_t n){
  return Aii::Memcmp(s1, s2, n);
}

}
