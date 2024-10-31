#include "x86_64/segments/segment_manager.hpp"
#include "x86_64/interrupts/structs/attributes.hpp"
#include "x86_64/interrupts/structs/gate_descriptor.hpp"
#include "x86_64/interrupts/structs/idt_table.hpp"
#include "x86_64/interrupts/structs/idt_descriptor.hpp"
#include "x86_64/interrupts/utilities.hpp"
#include "grains/grains.hpp"

namespace Grains::CPU{

  inline void IDT_Loading(){
    GRAIN("Tests to see if the idt will be correctly loaded",
      {
        kout << "PLEASE CHECK REGISTERS!\n";
        // segment initialisation - the same as grains/cpu/segment_manager.hpp
        using namespace X8664::SegmentAttributes;
        X8664::SegmentManager segmanager{};
        // try to reload the gdt
        segmanager.LoadCurrent();
        // try to reload the segment registers
        segmanager.ReloadSegmentRegisters();

        using namespace X8664::InterruptAttributes;
        X8664::IdtTable idt{};
        kout << intmode::hex << reinterpret_cast<std::uint64_t>(&idt) << '\n';
        X8664::IdtDescriptor idtr{idt};
        idtr.LoadCurrent();
      }
    );
  }

  // just something to set the IDT gate with
  void DoSomething(){}

  inline void IDT_Modification(){
    GRAIN("Tests to see if the updated idt contents will be correctly reflected",
      {
        kout << "PLEASE CHECK REGISTERS!\n";
        // segment initialisation - the same as grains/cpu/segment_manager.hpp
        using namespace X8664::SegmentAttributes;
        X8664::SegmentManager segmanager{};
        // try to reload the gdt
        segmanager.LoadCurrent();
        // try to reload the segment registers
        segmanager.ReloadSegmentRegisters();
        using namespace X8664::InterruptAttributes;
        X8664::IdtTable idt{};
        X8664::IdtDescriptor idtr{idt};
        idtr.LoadCurrent();
        idt[0] = X8664::GateDescriptor{
          X8664::InterruptAttributes::GateAttributes{
            .offset = reinterpret_cast<std::uint64_t>(&DoSomething),
            .segmentSelector = 0b1000,
            .isPresent = true,
          }
        };
      }
    );
  }

} // namespace Grains::CPU
