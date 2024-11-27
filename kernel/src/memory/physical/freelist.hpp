#ifndef MEMORY_PHYSICAL_FREELIST_HPP 
#define MEMORY_PHYSICAL_FREELIST_HPP

// This free list is implemented as a singly linked list
//
// It threads the free list entry throughout the free pages, by 
// placing the list entry within the pages themselves

#include <cstdint>

#include "./../address.hpp"
#include "./../alignment.hpp"
#include "./../memory_map/memory_map_descriptor.hpp"
#include "lib/kassert.hpp"

#include "firmware/limine/limine.h"
#include "firmware/limine/requests.hpp"
#include "drivers/serial/kostream.hpp"
#include "lib/string.h"

namespace Mem::Phys{

class FreeList{
  public:

    using AddrType = kvirtaddr_t;

    struct ListEntry{
      ListEntry* next;
    };

    static_assert(sizeof(ListEntry) == 8);

  public:
    // ------------------------------------------------------ //
    //  Lifetime
    // ------------------------------------------------------ //


    explicit FreeList(const MemoryMapDescriptor& memMap) noexcept;
    ~FreeList() = default;  
    void PrintFreeBases() const noexcept;

    // ------------------------------------------------------ //
    //  PMM Interface
    // ------------------------------------------------------ //

    [[nodiscard]] void* AllocatePage() noexcept;
    void FreePage(void* base) noexcept;

  private:
    void Initialise(const MemoryMapDescriptor& memMap) noexcept;

  private:
    ListEntry m_head;
    std::size_t m_qty;
};

} // namespace Mem::Phys

#endif
