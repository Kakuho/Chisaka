#pragma once

#include <cstdint>
#include <cassert>

#include "aii/array.hpp"

namespace Fs::Merofs{
  struct [[gnu::packed]] DataBlock{
    public:
      struct ListEntry{
        std::uint64_t prev;
        std::uint64_t next;
      };

    public:
      bool used;
      union{
        ListEntry links;
        Aii::Array<std::uint8_t, 500> buffer;
      };
  };
  static_assert(sizeof(DataBlock) <= 512);
}
