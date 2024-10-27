#include "cpu/segments/segment_manager.hpp"
#include "cpu/interrupts/structs/attributes.hpp"
#include "cpu/interrupts/structs/gate_descriptor.hpp"
#include "cpu/interrupts/structs/idt_table.hpp"
#include "cpu/interrupts/structs/idt_descriptor.hpp"
#include "cpu/interrupts/utilities.hpp"
#include "cpu/interrupts/interrupt_manager.hpp"

#include "grains/grains.hpp"

namespace Grains::CPU{

  // just something to set the IDT gate with
  inline void InterruptManager_InvokeInterrupt1(){
    GRAIN("Tests to see if the updated idt contents will be correctly reflected",
      {
        kout << "PLEASE CHECK REGISTERS!\n";
        // segment initialisation - the same as grains/cpu/segment_manager.hpp
        using namespace X8664::SegmentAttributes;
        X8664::SegmentManager segManager{};
        segManager.LoadCurrent();
        segManager.ReloadSegmentRegisters();
        // ------------------------------------------------------ //
        // segment manager 
        X8664::InterruptManager intManager{};
        intManager.EnableInterrupts();
        // now we can invoke the interrupts!
        X8664::invokeInterrupt(1);
      }
    );
  }

} // namespace Grains::CPU
