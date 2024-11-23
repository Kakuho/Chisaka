#include "freelist.hpp"

namespace Mem::Phys{

FreeList::FreeList(const MemoryMapDescriptor& memMap) noexcept
  : m_head{nullptr},
    m_qty{0}
{
  Initialise(memMap);
}

void FreeList::Initialise(const MemoryMapDescriptor& memMap) noexcept{
  // Initialisation Routine, sets up the free list entries
  bool firstIteration = true;
  for(std::uint8_t i = 0; i < memMap.Entries(); i++){
    const MemoryMapEntry& entry = memMap.Entry(i);
    if(entry.type == MemoryMapEntry::Type::Useable){
      AddrType current = entry.base;
      while(current < entry.base + entry.length){
        if(firstIteration){
          m_head.next = reinterpret_cast<ListEntry*>(current);
          firstIteration = false;
        }
        ListEntry* nextEntry = reinterpret_cast<ListEntry*>(current);
        if(current + 0x1000 < entry.base + entry.length){
          nextEntry->next = 
            reinterpret_cast<ListEntry*>(current + 0x1000);
          m_qty++;
        }
        current += 0x1000;
      }
    }
  }
}

void FreeList::PrintFreeBases() const noexcept{
  ListEntry* current = m_head.next;
  while(current->next != nullptr){
    kout << reinterpret_cast<std::uint64_t>(current) << '\n';
  }
}

// ------------------------------------------------------ //
//  PMM Interface
// ------------------------------------------------------ //

[[nodiscard]] void* FreeList::AllocatePage() noexcept{
  if(m_head.next != nullptr){
    void* allocd = m_head.next;
    m_head.next = m_head.next->next;
    return allocd;
  }
  else{
    return nullptr;
  }
}

void FreeList::FreePage(void* base) noexcept{
  std::uint64_t pagebase = Align<void*, 0x1000>(base);
  // set up the iteration loop
  ListEntry* current = m_head.next;
  std::uint64_t prev = reinterpret_cast<std::uint64_t>(current);
  std::uint64_t next = reinterpret_cast<std::uint64_t>(current->next);
  while(pagebase < next){
    current = current->next;
    prev = reinterpret_cast<std::uint64_t>(current);
    next = reinterpret_cast<std::uint64_t>(current->next);
  }
  // now we set up the page entry
  reinterpret_cast<ListEntry*>(prev)->next = 
    reinterpret_cast<ListEntry*>(pagebase);
  reinterpret_cast<ListEntry*>(pagebase)->next = 
    reinterpret_cast<ListEntry*>(next);
}

}
