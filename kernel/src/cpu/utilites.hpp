#ifndef U_UTIL_HPP
#define U_UTIL_HPP

// library for free standing cpu functions.

namespace X8664{

[[noreturn]]
inline void HaltCatchFire(){
  asm("cli");
  for(;;){
    asm("hlt");
  }
}

}

#endif
