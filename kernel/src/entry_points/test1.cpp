#include "drivers/serial/kostream.hpp"

// Extern declarations for global constructor array.

extern void (*__init_array[])();
extern void (*__init_array_end[])();

extern "C"{
  void invokeinterrupt1();
}

static void Test1(){
  // do your test1.cpp
  // put whatever tests you want here
  kout << "Entered Test1!" << '\n';
}

extern "C" void _start() {
  Test1();
}
