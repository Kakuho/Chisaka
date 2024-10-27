#include "gdt_descriptor.hpp"

namespace X8664{

//---------------------------------------------------------------//
//  Lifetime
//---------------------------------------------------------------//

GdtDescriptor::GdtDescriptor(
    std::uint16_t limit, 
    std::uint64_t offset
) noexcept:
  m_limit{limit},
  m_offset{offset}
{

}

GdtDescriptor::GdtDescriptor(
    SegmentAttributes::LimitOffsetArgs args
) noexcept:
  m_limit{args.limit},
  m_offset{args.offset}
{

}

GdtDescriptor::GdtDescriptor(GdtTable& gdtTable) noexcept:
  m_limit{
    static_cast<std::uint16_t>(gdtTable.Size())
  }, 
  m_offset{reinterpret_cast<std::uint64_t>(&gdtTable)}
{

}

//---------------------------------------------------------------//
// Operational
//---------------------------------------------------------------//

void GdtDescriptor::SwitchGdt(GdtTable& gdtTable) noexcept
{
  m_limit = static_cast<std::uint16_t>(gdtTable.Size());
  m_offset = reinterpret_cast<std::uint64_t>(&gdtTable);
}

void GdtDescriptor::SwitchLoad(GdtTable& gdtTable) noexcept
{
  SwitchGdt(gdtTable);
  LoadCurrent();
}

}
