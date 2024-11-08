#include "string.h"

extern "C" {

// memcpy copies n bytes from src to dest
void *memcpy(void *dest, const void *src, std::size_t n) {
  std::uint8_t *pdest = static_cast<std::uint8_t *>(dest);
  const std::uint8_t *psrc = static_cast<const std::uint8_t *>(src);

  for (std::size_t i = 0; i < n; i++) {
      pdest[i] = psrc[i];
  }

  return dest;
}

// memset copies static_cast<unsigned char>(c) to first n bytes of s
void *memset(void *s, int c, std::size_t n) {
  std::uint8_t *p = static_cast<std::uint8_t *>(s);
  for (std::size_t i = 0; i < n; i++) {
      kout << "we doing something" << '\n';
      kout << "i = " << i << '\n';
      p[i] = static_cast<uint8_t>(c);
  }

  return s;
}

// sorta the same as memcpy
void *memmove(void *dest, const void *src, std::size_t n) {
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

// compares the first n bytes of s1 and s2
int memcmp(const void *s1, const void *s2, std::size_t n) {
  const std::uint8_t *p1 = static_cast<const std::uint8_t *>(s1);
  const std::uint8_t *p2 = static_cast<const std::uint8_t *>(s2);

  for (std::size_t i = 0; i < n; i++) {
      if (p1[i] != p2[i]) {
          return p1[i] < p2[i] ? -1 : 1;
      }
  }

  return 0;
}

}
