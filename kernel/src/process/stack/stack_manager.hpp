#pragma once

// Class to encapsulate the management of Stacks as a class.

#include "stack_descriptor.hpp"
#include "aii/array.hpp"
#include "kassert.hpp"

extern "C" void switch_stack(std::uint64_t rsp, std::uint64_t code);
extern "C" std::uint64_t read_rsp();

namespace Mem{

class StackManager final{
  // TODO : Once heap is implemented, allow the stack table to 
  //        be a dynamic array, reallocating when neccessary
  using paddr_t = std::uint64_t;
  static constexpr std::size_t ENTRIES = 10;

  using StackTableType = Aii::Array<StackDescriptor, ENTRIES>;

  public:
    explicit StackManager() noexcept;

    void SwitchStack(std::size_t index) noexcept;
    void SwitchDumpStack(std::size_t index) noexcept;

    StackDescriptor& operator[](std::size_t index) noexcept{
      return m_stackTable[index];
    }

  private:
    StackTableType m_stackTable;
    std::size_t m_currIndex;
};

} // namespace Mem
