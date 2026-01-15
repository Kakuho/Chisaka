#pragma once

// SuperBlock is the partition of the file system which contains metadata of the entire file system.

#include <cstdint>
#include <cassert>

#include "aii/array.hpp"

namespace Fs::Merofs{
  struct SuperBlock{
    SuperBlock();
    std::uint64_t totalPhysBlocks;
    std::uint64_t totalInodeBlocks;
    std::uint64_t totalInodes;
    std::uint64_t freeInodes;
    std::uint64_t usedInodes;
    std::uint64_t totalDataBlocks;
    std::uint64_t freeDataBlocks;
    std::uint64_t usedDataBlocks;

    // linked list to inodes
    std::uint64_t freeInodeHead;
    std::uint64_t freeDataBlockHead;
    Aii::Array<std::uint8_t, 512 - 8*10> padding;
  };

  static_assert(sizeof(SuperBlock) == 512);
}
