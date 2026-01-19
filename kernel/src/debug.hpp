#pragma once

#include "drivers/serial/kostream.hpp"

namespace Chisaka::Debug{
  namespace{
    inline constexpr bool DEBUG_PRINT_ON = false;
  }

  template<typename ...Args>
  void Print(Args... args){
    if constexpr(DEBUG_PRINT_ON){
      (kout << ... << args) < '\n';
    }
  }
}
