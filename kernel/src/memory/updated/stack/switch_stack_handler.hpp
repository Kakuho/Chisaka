#ifndef MEMORY_SWITCH_STACK_HANDLER_HPP 
#define MEMORY_SWITCH_STACK_HANDLER_HPP

//  The high level code for handling stack switching
//    first decodes the entry code, and then it performs the 
//    required function call

#include "serial/kostream.hpp"

extern "C" void SwitchStackHandler(void* topofstack);

namespace Mem{

} // namespace Mem


#endif
