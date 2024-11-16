
// Runtime test to test the correctness of the segment descriptor

#include "x86_64/segments/segment_manager.hpp"
#include "drivers/serial/kostream.hpp"
#include "grains/grains.hpp"
#include "lib/kassert.hpp"

namespace Grains::CPU{

  inline void SegmentManager_Switches(){
    // PLEASE CHECK REGISTERS!
    GRAIN("Segment Manager Tests",
      {
        using namespace X8664::SegmentAttributes;
        X8664::SegmentManager segmanager{};
        // try to reload the gdt
        segmanager.LoadCurrent();
        // try to reload the segment registers
        segmanager.ReloadSegmentRegisters();

        // YOU NEED TO CHECK THE REGISTERS THAT:
        //
        //    THE GDT table has changed
        //
        //    THE GDTR has also changed
      }
    );
  }

} // namespace Grains::CPU
