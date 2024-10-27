#include "cpu/segments/segment_manager.hpp"
#include "cpu/interrupts/structs/attributes.hpp"
#include "cpu/interrupts/structs/gate_descriptor.hpp"
#include "cpu/interrupts/structs/idt_table.hpp"
#include "cpu/interrupts/structs/idt_descriptor.hpp"
#include "cpu/interrupts/interrupt_manager.hpp"
#include "cpu/interrupts/utilities.hpp"
#include "grains/grains.hpp"

namespace Grains::CPU{

  inline void Reference_OOB_Vector(){
    GRAIN("On OOB, the processor should #GP",
      {
        kout << "PLEASE CHECK INTERRUPT CODE!\n";
        // segment initialisation - the same as grains/cpu/segment_manager.hpp
        using namespace X8664::SegmentAttributes;
        X8664::SegmentManager segmanager{};
        segmanager.LoadCurrent();
        segmanager.ReloadSegmentRegisters();
        // ------------------------------------------------------ //
        using namespace X8664::InterruptAttributes;
        X8664::IdtTable idt{};
        // ------------------------------------------------------ //
        // idtr
        X8664::IdtDescriptor idtr{
          16, &idt
        };
        idtr.LoadCurrent();
        X8664::invokeInterrupt(1);
      }
    );
  }

  void DoSomething();

  inline void Reference_NonCanonicalOffset(){
    GRAIN("If we have a non canonical offset, we should error",
      {
        kout << "PLEASE CHECK INTERRUPT CODE!\n";
        // segment initialisation - the same as grains/cpu/segment_manager.hpp
        using namespace X8664::SegmentAttributes;
        X8664::SegmentManager segmanager{};
        segmanager.LoadCurrent();
        segmanager.ReloadSegmentRegisters();
        // ------------------------------------------------------ //
        using namespace X8664::InterruptAttributes;
        X8664::InterruptManager intManager{};
        /*
        void* nonCanonOffset = reinterpret_cast<void*>(
            reinterpret_cast<std::uint64_t>(&DoSomething) & 0xFFFFFF
        );
        intManager.InstallISR(30, nonCanonOffset);
        */
        intManager.InstallISR(30, &DoSomething);
        X8664::invokeInterrupt(30);
      }
    );
  }

  inline void ContextSwitching(){
    GRAIN("Program to Demonstrate Context Switching",
      {
        kout << "PLEASE CHECK INTERRUPT CODE!\n";
        // segment initialisation - the same as grains/cpu/segment_manager.hpp
        using namespace X8664::SegmentAttributes;
        X8664::SegmentManager segmanager{};
        segmanager.LoadCurrent();
        segmanager.ReloadSegmentRegisters();
        // ------------------------------------------------------ //
        using namespace X8664::InterruptAttributes;
        X8664::InterruptManager intManager{};
        X8664::invokeInterrupt(5);
      }
    );
  }

} // namespace Grains::CPU
