#include "stack_manager.hpp"

namespace Mem{

StackManager::StackManager() noexcept:
  m_currIndex{0}
{

}

void StackManager::SwitchStack(std::size_t index) noexcept{
  kassert(index < ENTRIES);
  m_stackTable[m_currIndex].lastPointer = read_rsp();
  switch_stack(reinterpret_cast<void*>(m_stackTable[index].lastPointer));
}

} // namespace Mem
