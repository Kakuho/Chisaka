#pragma once

// SuperBlock is the partition of the file system which contains metadata of the entire file system.

#include <cstdint>
#include <cassert>

#include "aii/array.hpp"

namespace Fs::Merofs{
  struct SuperBlock{
    SuperBlock();
    std::uint64_t totalFsBlocks;
    std::uint64_t totalInodeBlocks;
    std::uint64_t totalfreeInodes;
    std::uint64_t totalfreeDataBlocks;

    // linked list to inodes
    std::uint64_t freeInodeHead;
    std::uint64_t freeDataBlockHead;
    Aii::Array<std::uint8_t, 500 - 40> padding;
  };

  static_assert(sizeof(SuperBlock) == 512);
}
