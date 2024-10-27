#include "idt_descriptor.hpp"

namespace X8664{

IdtDescriptor::IdtDescriptor(IdtTable& table) noexcept:
  m_limit{
    static_cast<std::uint16_t>(table.Size() - 1)
  }, 
  m_offset{
    reinterpret_cast<std::uint64_t>(&table)
  }
{
}

IdtDescriptor::IdtDescriptor(
    std::uint16_t limit, 
    std::uint64_t offset
) noexcept:
  m_limit{limit},
  m_offset{offset}
{
  kassert(m_limit == limit);
  kassert(m_offset == offset);
}

//---------------------------------------------------------------//
// Operational
//---------------------------------------------------------------//

void IdtDescriptor::SwitchIdt(IdtTable& idt) noexcept{
  m_limit = static_cast<std::uint16_t>(idt.Size() - 1);
  m_offset = reinterpret_cast<std::uint64_t>(&idt);
}

void IdtDescriptor::SwitchLoad(IdtTable& table) noexcept{
  SwitchIdt(table);
  LoadCurrent();
}

}
