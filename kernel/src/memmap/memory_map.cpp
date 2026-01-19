#include "memory_map.hpp"
#include "kcontext.hpp"

namespace Chisaka{

void MemoryMap::Init() noexcept{
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

MemoryMap::MemoryMap(std::initializer_list<MemoryMapEntry> src) noexcept{
  Init(src);
}

void MemoryMap::Init(std::initializer_list<MemoryMapEntry> src) noexcept
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

void MemoryMap::LimineInitialise() noexcept{
  // Procedure to initialise the entry array at boot, using the Limine Protocol
  kassert(Limine::memorymap_request.response != nullptr);

  std::uint64_t mem_entries_t = Limine::memorymap_request.response->entry_count;
  kassert(mem_entries_t < MAX_ENTRIES);

  limine_memmap_entry** entries = Limine::memorymap_request.response->entries;
  for(std::uint64_t i = 0; i < mem_entries_t; i++){
    MemoryMapEntry::Type type = ConvertLimineType(entries[i]->type);
    kassert(type != MemoryMapEntry::Type::Unknown);
    // now populate the entry
    m_entries[i] = MemoryMapEntry{
      .base = entries[i]->base,
      .length = entries[i]->length,
      .type = type
    };
  }
}

void MemoryMap::InitialiseUseableData() noexcept{
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
  
PhysAddr MemoryMap::HighestUseableAddress() const noexcept{
  using enum MemoryMapEntry::Type;
  Chisaka::PhysAddr top = 0;
  for(std::uint64_t i = 0; i < m_entries.Size(); i++){
    if(m_entries[i].type != Empty){
      continue;
    }
    else{
      top = m_entries[i-1].base + m_entries[i-1].length - 1;
      break;
    }
  }
  return top;
}

PhysAddr MemoryMap::LowestUseableAddress() const noexcept{
  return m_entries[0].base;
}

Chisaka::PhysAddr
MemoryMap::LongestUseableBase() const noexcept{
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

std::size_t MemoryMap::UseableEntries() const noexcept{
  using enum MemoryMapEntry::Type;
  std::size_t count = 0;
  for(std::uint64_t i = 0; i < m_entries.Size(); i++){
    // now perform the comparison
    if(m_entries[i].type == Useable){
      count++;
    }
  }
  return count;
}

//-------------------------------------------------------------
//  Printing Functionalities
//-------------------------------------------------------------

void MemoryMap::Print() const noexcept{
  // Prints in the format:
  //  paddr \t vaddr \t length \t type
  for(std::uint64_t i = 0; i < m_entries.Size(); i++){
    MemoryMapEntry entry = m_entries[i];
    kout << intmode::hex 
         << entry.base << '\t' 
         << Chisaka::KContext::Get().PhysToVirtAddr(entry.base) << '\t'
         << entry.length << '\t' 
         << MemTypeToString(entry.type) << '\n';
  }
}

void MemoryMap::PrintPageFrames() const noexcept{
  // Prints in the format (index, base, end)
  using enum MemoryMapEntry::Type;
  std::size_t pindex = 0;
  for(std::uint64_t i = 0; i < m_entries.Size(); i++){
    // now perform the comparison
    if(m_entries[i].type == Useable){
      std::uint64_t base = m_entries[i].base;
      std::uint64_t length = m_entries[i].length;
      for(Chisaka::PhysAddr paddr = base; paddr < base + length; paddr += 0x1000){
        kout << "(" << pindex++  << ", "
                    << paddr    << ", "
                    << (paddr + 0x1000) - 1
                    << ")" << '\n';
      }
    }
  }
}

}
