#include "switch_stack_handler.hpp"

//  ABI of Stack Switching:
//
//    * Switches the Stack Top of stack to the new base
//    
//    * Pushses the %rip first
//
//    * Pushes a code which defines what function to perform during
//      a stack switch

#include <cstdint>

extern "C" void SwitchStackHandler(void* topofstack){
  const std::uint64_t* pCode = 
     reinterpret_cast<std::uint64_t*>(
         reinterpret_cast<std::uint64_t>(topofstack) - 16
      );
  kout << "value = " << *pCode << '\n';
  kout << "we stack switching baby!" << '\n';
}
