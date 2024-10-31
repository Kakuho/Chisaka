#include "idt_table.hpp"

namespace X8664{

//-------------------------------------------------------------
//  Operator Overloads
//-------------------------------------------------------------

[[nodiscard]] GateDescriptor& 
IdtTable::operator[](std::size_t index) noexcept{
  kassert(index < 256);
  return m_entries[index];
}

[[nodiscard]] const GateDescriptor& 
IdtTable::operator[](std::size_t index) const noexcept{
  kassert(index < 256);
  return m_entries[index];
}

}
