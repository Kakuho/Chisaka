#include "drivers/serial/kostream.hpp"
#include "firmware/limine/requests.hpp"
#include "x86_64/utilites.hpp"
#include "kassert.hpp"

static constexpr std::uint8_t QEMU_SUCCESS = 1;
static constexpr std::uint8_t QEMU_FAIL = 2;

// Extern declarations for global constructor array.

extern void (*__init_array[])();
extern void (*__init_array_end[])();

extern "C"{
  void invokeinterrupt1();
}

extern "C"{
  void outb(int port_addr, char val);
}

void ExitQemu(std::uint8_t code){
  outb(0xf4, code);
}

static void Test1(){
  // do your test1.cpp
  // put whatever tests you want here
  kout << "Entered Test1!" << '\n';
  kout << "Entered Test1!" << '\n';
  kout << "Entered Test1!" << '\n';
  kout << "Entered Test1!" << '\n';
  kout << "Entered Test1!" << '\n';
  kout << "Entered Test1!" << '\n';
  kout << "Entered Test1!" << '\n';
  kout << "what!" << '\n';
  ExitQemu(QEMU_FAIL);
  while(true){

  }
}

extern "C" void _start() {
  if(limine::is_limine_supported()){
    X8664::HaltCatchFire();
  }
  Test1();
}
