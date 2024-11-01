#ifndef MEMORY_STACK_DESCRIPTOR_HPP
#define MEMORY_STACK_DESCRIPTOR_HPP

//  Class to help management of Stacks.
//
//  A Stack Descriptor describes a particular stack that we can 
//  use

#include <cstdint>

namespace Mem{

using paddr_t = std::uint64_t;

struct PBaseLimit{
  paddr_t base;
  std::size_t limit;
};

class StackDescriptor{
  public:
    explicit StackDescriptor() noexcept = default;
    explicit StackDescriptor(PBaseLimit&& args) noexcept;

  private:
    paddr_t m_base;
    paddr_t m_lastAddr;
    std::size_t m_size;
};

} // namespace Mem

#endif
