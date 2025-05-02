#pragma once

#include "drivers/serial/kostream.hpp"

namespace Debug{
  namespace{
    inline constexpr bool DEBUG_PRINT_ON = false;
  }

  template<typename ...Args>
  void Print(Args... args){
    (kout << ... << args) < '\n';
  }
}
