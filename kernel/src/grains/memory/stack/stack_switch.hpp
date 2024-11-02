#include "memory/updated/stack/stack_manager.hpp"
#include "memory/updated/stack/stack_descriptor.hpp"
#include "grains/grains.hpp"
#include "include/kassert.hpp"
#include "serial/kostream.hpp"
#include "x86_64/utilites.hpp"

extern "C" void* stack_switch_read_memory();
extern "C" void* stack_switch_dump_memory();
extern "C" std::uint64_t read_rsp();

namespace Grains::Mem{

  inline void StackSwitching(){
    // PLEASE CHECK REGISTERS!
    GRAIN("Simple Program to demonstrate stack switching",
      {
        // for now I use hard coded base address ... 
        //
        // I would prefer to make a MemoryMapDescriptor class ...
        // but for now this wil do.
        ::Mem::StackManager manager; 
        manager[1] = ::Mem::StackDescriptor{
          {

            0xFFFF800000050000,
            0xFFFFF
          }
        };
        REQUIRE_EQUAL(manager[1].LastAddr(), 0xFFFF800000050000);

        manager[2] = ::Mem::StackDescriptor{
          {

            0xFFFF800000080000,
            0xFFFFF
          }
        };

        REQUIRE_EQUAL(manager[2].LastAddr(), 0xFFFF800000080000);

        // now we perform the switching code

        manager.SwitchStack(1);
        stack_switch_dump_memory();

        /*
        manager.SwitchStack(2);
        REQUIRE_EQUAL(manager[1].LastAddr(), 0xFFFF800000050018);

        manager.SwitchStack(1);
        */

        //void* topofstack = stack_switch_read_memory();
        //REQUIRE_EQUAL(*(static_cast<std::uint64_t*>(topofstack)), 10);
        //X8664::HaltCatchFire();
      }
    );
  }

} // namespace Grains::CPU
