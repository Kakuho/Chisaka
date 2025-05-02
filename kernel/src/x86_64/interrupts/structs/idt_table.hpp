#pragma once

// Idt table
// 
// Reference: Intel vol3-6.10

#include <cstdint>

#include "aii/array.hpp"

#include "lib/kassert.hpp"
#include "x86_64/utilites.hpp"
#include "gate_descriptor.hpp"

namespace X8664{

constexpr std::uint16_t GetIdtIndexer(std::uint8_t index){
  return index * 16;
}

class IdtTable{
  // check the gdt_table.hpp
  static inline constexpr std::uint16_t KERNEL_CODE_SEGMENT_INDEX = 0b1'000;

  public:
    //-------------------------------------------------------------
    //  Lifetime
    //-------------------------------------------------------------

    explicit IdtTable() = default;
    explicit IdtTable(IdtTable& src) = default;
    IdtTable& operator=(IdtTable& other) = default;

    //-------------------------------------------------------------
    //  Queries
    //-------------------------------------------------------------

    [[nodiscard]] constexpr std::size_t Size() const noexcept
    { return sizeof(m_entries); }

    //-------------------------------------------------------------
    //  Operator Overloads
    //-------------------------------------------------------------

    [[nodiscard]] GateDescriptor& operator[](std::size_t index) noexcept;
    [[nodiscard]] const GateDescriptor& operator[](std::size_t index) const noexcept;

  private:
    Aii::Array<GateDescriptor, 256> m_entries;
};

static_assert(sizeof(IdtTable) == 256*16);

} // namespace X8664
