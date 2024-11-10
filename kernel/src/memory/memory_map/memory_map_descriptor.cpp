#include "memory_map_descriptor.hpp"

namespace Mem{

MemoryMapDescriptor::MemoryMapDescriptor() noexcept{
  using enum MemoryMapEntry::Type;
  for(std::size_t i = 0; i < m_entries.Size(); i++){
    m_entries[i] = MemoryMapEntry{
      0, 
      0, 
      Empty
    };
  }
  LimineInitialise();
  // SORT(m_entries)
  InitialiseUseableData();
}

MemoryMapDescriptor::MemoryMapDescriptor(
  std::initializer_list<MemoryMapEntry> src
) noexcept
{
  using enum MemoryMapEntry::Type;
  kassert(src.size() < MAX_ENTRIES);
  // empty the array first
  for(std::size_t i = 0; i < m_entries.Size(); i++){
    m_entries[i] = MemoryMapEntry{
      0, 
      0, 
      Empty
    };
  }
  // now populate it
  for(std::size_t i = 0; i < src.size(); i++){
    m_entries[i] = src.begin()[i];
  }
  // SORT(m_entries)
  InitialiseUseableData();
}

void MemoryMapDescriptor::LimineInitialise() noexcept{
  // Procedure to initialise the entry array at boot, using the Limine Protocol
  using namespace limine::requests;
  using enum MemoryMapEntry::Type;
  if(memorymap_request.response == nullptr){
    kout << "MEM FAIL\n";
  }
  std::uint64_t mem_entries_t = memorymap_request.response->entry_count;
  kassert(mem_entries_t < MAX_ENTRIES);
  limine_memmap_entry** entries = memorymap_request.response->entries;
  for(std::uint64_t i = 0; i < mem_entries_t; i++){
    MemoryMapEntry::Type type = ConvertLimineType(entries[i]->type);
    kassert(type != Unknown);
    // now populate the entry
    m_entries[i] = MemoryMapEntry{
      .base = entries[i]->base,
      .length = entries[i]->length,
      .type = type
    };
  }
}

void MemoryMapDescriptor::InitialiseUseableData() noexcept{
  using enum MemoryMapEntry::Type;
  std::size_t total = 0;
  for(std::uint64_t i = 0; i < m_entries.Size(); i++){
    if(m_entries[i].type == Useable){
      total += m_entries[i].length;
    }
  }
  m_totalUseableBytes = total;
}

//-------------------------------------------------------------
//  Queries
//-------------------------------------------------------------
  
[[nodiscard]] physaddr_t 
MemoryMapDescriptor::TopUseableAddress() const noexcept{
  using enum MemoryMapEntry::Type;
  physaddr_t top = 0;
  for(std::uint64_t i = 0; i < m_entries.Size(); i++){
    if(m_entries[i].type != Empty){
      continue;
    }
    else{
      top = m_entries[i-1].base + m_entries[i-1].length;
      break;
    }
  }
  return top;
}

[[nodiscard]] physaddr_t 
MemoryMapDescriptor::LowestUseableAddress() const noexcept{
  return m_entries[0].base;
}

[[nodiscard]] physaddr_t 
MemoryMapDescriptor::LongestUseableBase() const noexcept{
  using enum MemoryMapEntry::Type;
  std::uint8_t longest = 0;
  for(std::uint64_t i = 0; i < m_entries.Size(); i++){
    // now perform the comparison
    if(m_entries[i].type == Useable){
      if(m_entries[longest].length < m_entries[i].length){
        longest = i;
      }
    }
  }
  return m_entries[longest].base;
}

//-------------------------------------------------------------
//  Printing Functionalities
//-------------------------------------------------------------

void MemoryMapDescriptor::Print() const noexcept{
  // Prints in the format:
  //  paddr \t vaddr \t length \t type
  for(std::uint64_t i = 0; i < m_entries.Size(); i++){
    MemoryMapEntry entry = m_entries[i];
    kout << intmode::hex 
         << entry.base << '\t' 
         << PhysToKVirtAddr(entry.base) << '\t'
         << entry.length << '\t' 
         << MemTypeToString(entry.type) << '\n';
  }
}

void MemoryMapDescriptor::PrintPageFrames() const noexcept{
  // Prints in the format (index, base, end)
  using enum MemoryMapEntry::Type;
  std::size_t pindex = 0;
  for(std::uint64_t i = 0; i < m_entries.Size(); i++){
    // now perform the comparison
    if(m_entries[i].type == Useable){
      std::uint64_t base = m_entries[i].base;
      std::uint64_t length = m_entries[i].length;
      for(physaddr_t paddr = base; paddr < base + length; paddr += 0x1000){
        kout << "(" << pindex++  << ", "
                    << paddr    << ", "
                    << (paddr + 0x1000) - 1
                    << ")" << '\n';
      }
    }
  }
}

}
