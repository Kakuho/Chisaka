#pragma once

#include <cstdint>

namespace Chisaka{
  using PhysAddr = std::uintptr_t;
  using PhysAddr64 = std::uint64_t;
  using PhysAddr32 = std::uint32_t;

  using VirtAddr = std::uintptr_t;
  using VirtAddr64 = std::uint64_t;
  using VirtAddr32 = std::uint32_t;
}
