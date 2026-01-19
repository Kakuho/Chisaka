#pragma once

#include <cstdint>

namespace Mem::Page::Freelist{
  
enum class OPT: std::uint8_t{
  Z = 1,
  LOW = 1 << 1
};

}
