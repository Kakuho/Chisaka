// Runtime test to test the correctness of the segment descriptor

#include "x86_64/segments/structs/gdt_descriptor.hpp"
#include "x86_64/segments/structs/attributes.hpp"
#include "drivers/serial/kostream.hpp"
#include "grains/grains.hpp"
#include "kassert.hpp"

namespace Grains::CPU{

  inline void Gdt_Switches(){
    // PLEASE CHECK REGISTERS!
    GRAIN("SegmentDescriptor Default Constructor Tests",
      {
        using namespace X8664::SegmentAttributes;
        std::uint64_t offset = 0xDEAD'BEEF'0000'BEEC;
        X8664::GdtDescriptor descriptor{
          {
            .limit = 0xFFAB,
            .offset = offset
          }
        };
        REQUIRE_EQUAL(descriptor.Limit(), 0xFFAB)
        REQUIRE_EQUAL(descriptor.Offset(), offset)
        descriptor.LoadCurrent();

        // YOU NEED TO CHECK THE REGISTERS THAT:
        //
        //    THE OFFSET HAS SWITCHED
        //
        //    THE LIMIT HAS ALSO SWITCHED
      }
    );
  }

} // namespace Grains::CPU
