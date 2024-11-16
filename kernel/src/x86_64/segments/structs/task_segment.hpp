#ifndef TASK_SEGMENT_HPP
#define TASK_SEGMENT_HPP

// Class definition for the Task State Segment
//
// Notation:
//     RSPs - used to store stack pointers, 
//     pointers to the last stack entries
//
//     IST are pointers to interrupt stack tables, 
//     I believe they also point to the last interrupt stack entries
//
// Reference: Intel vol3-8.7

#include <cstdint>

#include "primrose/static_array.hpp"
#include "lib/kassert.hpp"

namespace X8664{

struct __attribute__((packed)) TaskSegment{
  using RspTableType = Prim::StaticArray<std::uint64_t, 3>;
  using IstTableType = Prim::StaticArray<std::uint64_t, 7>;

  public:
    //---------------------------------------------------------------//
    // Lifetime
    //---------------------------------------------------------------//

    TaskSegment():
      reserved0{0},
      reserved1{0},
      reserved3{0},
      reserved4{0}
    {

    }

    //---------------------------------------------------------------//
    // Indexers
    //---------------------------------------------------------------//
    
    std::uint64_t Rsp(std::uint8_t index){
      kassert(index < 3);
      return m_rsp_table[index];
    }

    std::uint64_t Ist(std::uint8_t index){
      kassert(index < 7);
      return m_ist_table[index];
    }

    std::uint16_t IOBitmap(){
      // IO Map Base Address
      return m_iopbm;
    }

  public:

    std::uint32_t reserved0;
    RspTableType m_rsp_table;
    std::uint64_t reserved1;
    IstTableType m_ist_table;
    std::uint64_t reserved3;
    std::uint16_t  reserved4;
    std::uint16_t m_iopbm;
};

// The Task Segment in 64-bit mode has to be 104 bytes

static_assert(sizeof(TaskSegment) == 104);

} // namespace X8664

#endif
