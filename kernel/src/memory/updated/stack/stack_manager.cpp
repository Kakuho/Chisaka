#include "stack_manager.hpp"

namespace Mem{

StackManager::StackManager() noexcept:
  m_currIndex{0}
{

}

void StackManager::SwitchStack(std::size_t index) noexcept{
  kassert(index < ENTRIES);
  m_stackTable[m_currIndex].LastAddr() = read_rsp();
  m_currIndex = index;
  switch_stack(m_stackTable[index].LastAddr());
}

} // namespace Mem
