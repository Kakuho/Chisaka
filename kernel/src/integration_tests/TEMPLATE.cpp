#include "drivers/serial/kostream.hpp"
#include "firmware/limine/requests.hpp"
#include "x86_64/utilites.hpp"
#include "kassert.hpp"

static constexpr std::uint8_t QEMU_SUCCESS = 0;
static constexpr std::uint8_t QEMU_FAIL = 1;

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

static void Hanging(){
  ExitQemu(QEMU_FAIL);
}

extern "C" void _start() {
  if(limine::is_limine_supported()){
    X8664::HaltCatchFire();
  }
  for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
      __init_array[i]();
  }
  Hanging();
}
