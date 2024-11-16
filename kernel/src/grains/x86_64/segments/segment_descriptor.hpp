// Runtime test to test the correctness of the segment descriptor

#include "x86_64/segments/segment_manager.hpp"
#include "x86_64/segments/structs/segment_descriptor.hpp"
#include "x86_64/segments/structs/task_descriptor.hpp"
#include "drivers/serial/kostream.hpp"
#include "grains/grains.hpp"
#include "lib/kassert.hpp"

namespace Grains::CPU{

  inline void SegmentDescriptor_Default(){
    GRAIN("SegmentDescriptor Default Constructor Tests",
      {
        X8664::SegmentDescriptor segment{};
        REQUIRE(
          "access byte should be 0", 
          segment.m_accessbyte == 0
        );
        REQUIRE(
          "limit should be 0", 
          segment.SegmentLimit() == 0
        );
      }
    );
  }

  inline void SegmentDescriptor_Setters(){
    GRAIN("Segment Descriptor Settor Interface Tests",
      {
        // i find this clearer than setting it all in the constructor
        X8664::SegmentDescriptor segment{};
        segment.SetBaseAddress(reinterpret_cast<void*>(0x12345678));
        segment.SetSegmentLimit(0x11001);
        segment.SetAccessByte({
            .present = true, 
            .privilaged = true, 
            .isSystemSegment = false,
            .type = 0x0A
        });
        segment.SetFlags({
            .isByteGranular = false,
            .db = false,
            .longModeSegment = true,
            .avl = false
        });

        // checking internal data members
        
        REQUIRE_EQUAL(segment.m_limit0, 0x1001)
        REQUIRE_EQUAL(segment.m_base0,  0x12345678 & 0xFFFF)
        REQUIRE_EQUAL(segment.m_base1,  (0x12345678 >> 16) & 0xFF)
        REQUIRE_EQUAL(segment.m_base2,  (0x12345678 >> 24) & 0xFF)
        REQUIRE_EQUAL(segment.m_accessbyte,  0x9A)
        REQUIRE_EQUAL(segment.m_flags_limit1,  0xA1)

        // checking external interface
        
      }
    );
  }

  inline void SegmentDescriptor_ConstructorVer2(){
    GRAIN("Segment Descriptor Settor Interface Tests",
      {
        using namespace X8664::SegmentAttributes;
        X8664::SegmentDescriptor segment
        {
          BaseLimit
          {
            reinterpret_cast<void*>(0x12345678),
            0x11001,
          },
          AccessByte
          {
            .present = true, 
            .privilaged = true, 
            .isSystemSegment = false,
            .type = 0x0A
          },
          Flags
          {
            .isByteGranular = false,
            .db = false,
            .longModeSegment = true,
            .avl = false
          }
        };

        // checking internal data members

        REQUIRE_EQUAL(segment.m_limit0, 0x1001)
        REQUIRE_EQUAL(segment.m_base0,  0x12345678 & 0xFFFF)
        REQUIRE_EQUAL(segment.m_base1,  (0x12345678 >> 16) & 0xFF)
        REQUIRE_EQUAL(segment.m_base2,  (0x12345678 >> 24) & 0xFF)
        REQUIRE_EQUAL(segment.m_accessbyte,  0x9A)
        REQUIRE_EQUAL(segment.m_flags_limit1,  0xA1)

        // checking user friendly interface

        REQUIRE_EQUAL(segment.BaseAddress(), 0x12345678);
        REQUIRE_EQUAL(segment.SegmentLimit(), 0x11001);

        REQUIRE_EQUAL(segment.IsPresent(), true);
        REQUIRE_EQUAL(segment.IsPrivilaged(), true);

        REQUIRE_EQUAL(segment.IsSystemSegment(), false);

        REQUIRE_EQUAL(segment.IsByteGranular(), false);
        REQUIRE_EQUAL(segment.IsDB(), false);
        REQUIRE_EQUAL(segment.IsLongCodeSegment(), true);

      }
    );
  }
  
  inline void TaskDescriptor_Constructor(){
    GRAIN("Test the constructor for Tasks", {
        using namespace X8664::SegmentAttributes;
        std::uint64_t address = 0xABCD'EF01'2345'6789;
        X8664::TaskDescriptor tasktor
        {
          BaseLimit
          {

            reinterpret_cast<void*>(0xABCD'EF01'2345'6789),
            0x11001,
          },
          TaskAttributes
          {
            .isByteGranular = true,
            .avl = false,
            .present = true,
            .privilaged = true,
            .busy = true
          }
        };
      
        // testing internal data members
        // Internal Segment Descriptor
        REQUIRE_EQUAL(tasktor.m_segdesc.m_limit0, 0x1001)
        REQUIRE_EQUAL(tasktor.m_segdesc.m_base0,  address & 0xFFFF)
        REQUIRE_EQUAL(tasktor.m_segdesc.m_base1,  (address >> 16) & 0xFF)
        REQUIRE_EQUAL(tasktor.m_segdesc.m_base2,  (address >> 24) & 0xFF)
        REQUIRE_EQUAL(tasktor.m_segdesc.m_accessbyte,  0x8B)
        REQUIRE_EQUAL(tasktor.m_segdesc.m_flags_limit1,  0x01)
        // Task Descriptor Specific
        REQUIRE_EQUAL(tasktor.m_base3, address >> 32)
        REQUIRE_EQUAL(tasktor.m_reserved0, 0)
        REQUIRE_EQUAL(tasktor.m_res1_zeros, 0)
        REQUIRE_EQUAL(tasktor.m_res2, 0)

        // testing user friendly interface

        REQUIRE_EQUAL(tasktor.SegmentLimit(), 0x11001)
        REQUIRE_EQUAL(tasktor.BaseAddress(), address);
        // Access Byte
        REQUIRE_EQUAL(tasktor.IsPresent(),  true)
        REQUIRE_EQUAL(tasktor.IsPrivilaged(),  true)
        REQUIRE_EQUAL(tasktor.Type(),  0b1011)
        REQUIRE_EQUAL(tasktor.IsBusy(), true);
        // Flags
        REQUIRE_EQUAL(tasktor.IsByteGranular(), true)
    });
  }

} // namespace Grains::CPU
