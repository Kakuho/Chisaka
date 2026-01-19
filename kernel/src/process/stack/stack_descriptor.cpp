#include "stack_descriptor.hpp"

namespace Mem{

StackDescriptor::StackDescriptor(PBaseLimit&& args) noexcept:
  m_base{args.base},
  m_lastAddr{m_base},
  m_size{args.limit}
{
}

} // namespace Mem
