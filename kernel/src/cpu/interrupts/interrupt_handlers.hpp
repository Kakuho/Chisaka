#ifndef INTERRUPT_HANDLERS_HPP
#define INTERRUPT_HANDLERS_HPP

// Some cute interrupt handlers :D

#include <cstdint>

#include "serial/kostream.hpp"
#include "include/kassert.hpp"
#include "memory/memory.hpp"

//  the handlers are linked using C linkage, making it easier to call from assembly.

extern "C" {

void sayHi(int x);

void generalIRQ(int x);

void contextSwitchISR(void* stackaddr);

void PageFaultISR();

} // extern "C"

#endif

