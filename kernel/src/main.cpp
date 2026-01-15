#include <cstdint>

#include "firmware/limine/requests.hpp"
#include "x86_64/timers/hpet_controller.hpp"
#include "x86_64/utilites.hpp"
#include "memory/heap/tests/test.hpp"
#include "memory/heap/tests/page_size/test_run.hpp"

#include "drivers/ahci/class_interface/samples/identify_device.hpp"
#include "drivers/ahci/class_interface/samples/write_read.hpp"
#include "drivers/ahci/class_interface/samples/change_addressable_sectors.hpp"

#include "fs/merofs/samples/init.hpp"

#include "x86_64/timers/samples/hpet_poll.hpp"
#include "x86_64/timers/samples/tryhpet.hpp"

// Extern declarations for global constructor array.

extern void (*__init_array[])();
extern void (*__init_array_end[])();

extern "C"{
  void invokeinterrupt1();
}

extern "C" void _start() {
  // if limine is not supported, then kill the system
  if(limine::is_limine_supported()){
    X8664::HaltCatchFire();
  }

  // Call global constructors.
  for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
      __init_array[i]();
  }

  //X8664::Timers::Samples::TryHpet::TryOneShot();
  //Drivers::Ahci::Class::Sample::WriteRead_AhciFuncs();
  //Drivers::Ahci::Class::Sample::WriteRead_AhciFuncsMany();
  //Drivers::Ahci::Class::Sample::WriteInterrupt();
  //Drivers::Ahci::Class::Sample::IdentifyDevice_MemFun();
  //Drivers::Ahci::Class::Sample::PrintMaxAddress();
  Fs::Merofs::Samples::Initialise();
  //Fs::Merofs::Samples::Hehe();
  //Drivers::Ahci::Class::Sample::IdentifyDevice();
  //Drivers::Ahci::Class::Sample::Write_OutOfBounds();

  //kout << "Rarity is cute pony" << '\n';
  X8664::HaltCatchFire();
}
