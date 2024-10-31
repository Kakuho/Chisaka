#ifndef MEMORY_PHYSICAL_BUMP_HPP 
#define MEMORY_PHYSICAL_BUMP_HPP

// The bump allocator is just a bootstrapping allocator, whose 
// lifetime is short lived.
//
// It should be used in conjuction with other Physical Page Frame 
// Allocators.
//
// The range is inclusive, i.e [start, end] is the allocatable range,
// adjusted for alignment

#include <cstdint>

#include "./../address.hpp"
#include "./../alignment.hpp"
#include "include/kassert.hpp"

namespace Mem::Phys{

class Bump{
  public:

    using AddrType = physaddr_t;

  public:
    // ------------------------------------------------------ //
    //  Lifetime
    // ------------------------------------------------------ //

    explicit Bump(AddrType start, AddrType end) noexcept;
    ~Bump() = default;  
    
    // ------------------------------------------------------ //
    //  PMM Interface
    // ------------------------------------------------------ //

    [[nodiscard]] AddrType AllocatePage() noexcept;

    // A bump allocator does not free, however this is implemented just 
    // to satisfy the PMM Concept
    void FreePage([[maybe_unused]] AddrType base) noexcept{}

    // ------------------------------------------------------ //
    //  Useful for the Bump Allocator
    // ------------------------------------------------------ //

    [[nodiscard]] AddrType Start() noexcept { return m_start;}
    [[nodiscard]] AddrType End() noexcept { return m_end;}

  private:
    AddrType m_start;
    AddrType m_end;
};

} // namespace Mem::Phys

#endif
