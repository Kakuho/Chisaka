#include "cpu/segments/segment_manager.hpp"
#include "cpu/interrupts/structs/attributes.hpp"
#include "cpu/interrupts/structs/gate_descriptor.hpp"
#include "cpu/interrupts/structs/idt_table.hpp"
#include "cpu/interrupts/structs/idt_descriptor.hpp"
#include "cpu/interrupts/utilities.hpp"
#include "grains/grains.hpp"

namespace Grains::CPU{

  // just something to set the IDT gate with
  void DoSomething(){}

  inline void IDTR_Interface_Tests(){
    GRAIN("Tests to see if the idtr interface works as expected",
      {
        std::uint64_t offset = 0xabcd'1234'abcd'5678;
        std::uint16_t limit = 0x1234;
        X8664::IdtDescriptor idtr{limit, offset};

        // internal data members
        REQUIRE_EQUAL(idtr.m_offset, offset);
        REQUIRE_EQUAL(idtr.m_limit, limit);

        // interface
        REQUIRE_EQUAL(idtr.Offset(), offset);
        REQUIRE_EQUAL(idtr.Limit(), limit);
      }
    );
  }


  inline void IDTR_Modification(){
    GRAIN("Tests to see if the updated idt contents will be correctly reflected",
      {
        using namespace X8664::SegmentAttributes;
        kout << "PLEASE CHECK REGISTERS!\n";
        // segment initialisation - the same as grains/cpu/segment_manager.hpp
        X8664::SegmentManager segmanager{};
        segmanager.LoadCurrent();
        segmanager.ReloadSegmentRegisters();
        // ------------------------------------------------------ //
        // what we're actually testing
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
