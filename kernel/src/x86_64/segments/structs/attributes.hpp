#ifndef SEGMENT_ATTRIBUTES_HPP
#define SEGMENT_ATTRIBUTES_HPP

// structs for segment related classes constructor arguments

#include <cstdint>

namespace X8664::SegmentAttributes{

  struct BaseLimit
  {
    void* base;
    std::uint32_t limit;
  };

  // argument packs for access byte (bits 15 ... 11)
  struct AccessByte
  {
    bool present;
    bool privilaged;
    bool isSystemSegment;         
    std::uint8_t type;
    static AccessByte Null();
  };

  // flag pack for bits 23 ... 20
  struct Flags
  {
    bool isByteGranular;
    bool db;
    bool longModeSegment;
    bool avl;

    // conversions
    static Flags Null();
  };

  // for the task segment descriptor
  struct TaskAttributes
  {
    bool isByteGranular;
    bool avl;
    bool present;
    bool privilaged;
    bool busy;
  };

  // for registers
  struct LimitOffsetArgs{
    std::uint16_t limit;
    std::uint64_t offset;
  };

}

#endif
