#pragma once

#include <cstdint>

namespace Chisaka{
  
enum class RamOptions: std::uint8_t{
  Z = 1,
  LOW = 1 << 1
};

}
