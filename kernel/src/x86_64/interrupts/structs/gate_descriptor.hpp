#ifndef GATE_DESCRIPTOR_HPP
#define GATE_DESCRIPTOR_HPP

// Class to represent a IDT 64-bit Gate Descriptor
//
// Reference: Intel vol3-6.14.1

#include <cstdint>
#include <cassert>

#include "include/kassert.hpp"
#include "attributes.hpp"
#include "./../constants.hpp"

namespace X8664{
  class GateDescriptor{
    public:
      //---------------------------------------------------------------//
      //  Lifetime 
      //---------------------------------------------------------------//

      explicit GateDescriptor();

      explicit GateDescriptor(
          std::uint64_t offset, 
          std::uint16_t segmentSelector,
          bool present
      ) noexcept;

      explicit GateDescriptor(
          InterruptAttributes::GateAttributes args
      ) noexcept;

      //---------------------------------------------------------------//
      //  Observers
      //---------------------------------------------------------------//
      
      [[nodiscard]] std::uint64_t Offset() const noexcept;
      [[nodiscard]] std::uint16_t SegmentSelector() const noexcept ;
      [[nodiscard]] std::uint8_t Ist() const noexcept ;
      [[nodiscard]] std::uint8_t Type() const noexcept ;
      [[nodiscard]] bool IsPivilaged() const noexcept;
      [[nodiscard]] bool IsPresent() const noexcept;

      //---------------------------------------------------------------//
      //  Mutators
      //---------------------------------------------------------------//

      // for the frontend, modifiable by the user
      void SetOffset(void* offset);           
      void SetOffset(std::uint64_t offset);
      void SetSegmentSelector(std::uint16_t segSelector);
      void SetPresent();
      void ClearPresent();

    private:
      // private - these are fixed and not modifiable

      void SetPrivilageLevel();
      void SetSegmentType();

    public:
      // internal representation
      std::uint16_t   m_offset0;
      std::uint16_t   m_segmentSelector;
      std::uint8_t    m_zeros_ist;
      std::uint8_t    m_accessByte;  // this is access + type packed
      std::uint16_t   m_offset1;
      std::uint32_t   m_offset2;
      std::uint32_t   m_reserved;
  };

// size of the 64 bit interrupt call gate descriptor is 
// 16 bytes [32 + 32 + 32 + 32 bits]

static_assert(sizeof(GateDescriptor) == 16);

} // X8664

#endif
