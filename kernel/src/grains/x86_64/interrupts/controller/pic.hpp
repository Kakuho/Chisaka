#include "x86_64/segments/segment_manager.hpp"
#include "x86_64/interrupts/interrupt_manager.hpp"
#include "x86_64/interrupts/controllers/pic.hpp"

#include "grains/grains.hpp"

namespace Grains::CPU{

  inline void PIC_DefaultCtor(){
    GRAIN("Smoke Test for PIC",
      {
        // ------------------------------------------------------ //
        // SETUP
        // segment initialisation - the same as grains/cpu/segment_manager.hpp
        using namespace X8664::SegmentAttributes;
        X8664::SegmentManager segManager{};
        segManager.LoadCurrent();
        segManager.ReloadSegmentRegisters();
        // ------------------------------------------------------ //
        // Interrupts
        X8664::InterruptManager intManager{};
        intManager.EnableInterrupts();
        // END OF SETUP
        // ------------------------------------------------------ //
        X8664::Interrupt::PicController pic{0xA0, 0xC0};
        pic.MaskIrq(0);
        pic.MaskIrq(3);
      }
    );
  }

} // namespace Grains::CPU
