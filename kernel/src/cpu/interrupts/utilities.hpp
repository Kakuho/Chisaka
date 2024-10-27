#ifndef INTERRUPTS_UTILITIES_HPP
#define INTERRUPTS_UTILITIES_HPP

#include <cstdint>

extern "C" void clear_interrupts();
extern "C" void invokeinterrupt1();
extern "C" void invokeinterrupt2();
extern "C" void invokeinterrupt3();
extern "C" void invokeinterrupt4();
extern "C" void invokeinterrupt5();
extern "C" void invokeinterrupt6();
extern "C" void invokeinterrupt30();
extern "C" void invokeinterrupt33();

namespace X8664{

  inline void invokeInterrupt(std::uint8_t no){
    switch(no){
      case 1:
        invokeinterrupt1();
        break;
      case 5:
        invokeinterrupt5();
        break;
      case 30:
        invokeinterrupt30();
        break;
    }
  }
}

#endif
