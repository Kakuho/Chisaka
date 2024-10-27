#include "attributes.hpp"

namespace X8664::SegmentAttributes{


// ------------------------------------------------------ //
// Access Byte
// ------------------------------------------------------ //

AccessByte AccessByte::Null(){
  return AccessByte {
    .present = false,
    .privilaged = true,
    .isSystemSegment = true,
    .type = 0
  };
}

// ------------------------------------------------------ //
// Flags
// ------------------------------------------------------ //

Flags Flags::Null(){
  return Flags{
    .isByteGranular = true,
    .db = false,
    .longModeSegment = false,
    .avl = false
  };
}

} // namespace X8664::SegmentAttributes
