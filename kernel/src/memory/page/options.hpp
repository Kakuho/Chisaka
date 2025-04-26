#pragma once

#include <cstdint>

namespace Mem::Page{
  
enum class OPT: std::uint8_t{
  Z = 1,
  LOW = 1 << 1
};

}
