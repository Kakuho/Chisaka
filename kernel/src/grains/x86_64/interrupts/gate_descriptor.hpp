// Runtime test to test the correctness of the segment descriptor

#include "x86_64/interrupts/structs/attributes.hpp"
#include "x86_64/interrupts/structs/gate_descriptor.hpp"
#include "grains/grains.hpp"

namespace Grains::CPU{

  inline void InterruptDescriptor_tests(){
    // PLEASE CHECK REGISTERS!
    GRAIN("SegmentDescriptor Default Constructor Tests",
      {
        using namespace X8664::InterruptAttributes;
        std::uint64_t address = 0xabcdef'123456;
        X8664::GateDescriptor gate{
          GateAttributes{
            .offset = address,
            .segmentSelector = 0x1234,
            .isPresent = true
          }
        };

        // testing internal data members

        REQUIRE_EQUAL(gate.m_offset0, address&0xFFFF);
        REQUIRE_EQUAL(gate.m_offset0, 0x3456);
        REQUIRE_EQUAL(gate.m_offset1, (address>>16)&0xFFFF);
        REQUIRE_EQUAL(gate.m_offset1, 0xef12);
        REQUIRE_EQUAL(gate.m_offset2, address>>32);
        REQUIRE_EQUAL(gate.m_offset2, 0xabcd);
        REQUIRE_EQUAL(gate.m_segmentSelector, 0x1234);

        // Acces Byte and Flags
        REQUIRE_EQUAL(gate.m_accessByte & 0x0F, 0b1110);
        REQUIRE_EQUAL(gate.m_accessByte & 0xF0, 0x80);

        // testing user friendly interface data members

        REQUIRE_EQUAL(gate.Offset(), 0xAB'CDEF'1234'56ul);
        REQUIRE_EQUAL(gate.SegmentSelector(), 0x1234);

        REQUIRE_EQUAL(gate.Type(), 0b1110);

        REQUIRE_EQUAL(gate.IsPresent(), true);
        REQUIRE_EQUAL(gate.IsPivilaged(), true);
      }
    );
  }

} // namespace Grains::CPU
