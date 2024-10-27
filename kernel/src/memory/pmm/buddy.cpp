#include "buddy.hpp"
#include <cstdint>
#include <cstdio>

namespace memory::pmm::buddy{

  PhysicalMemory::PhysicalMemory(){
    PopulateRanges();
  }

  void PhysicalMemory::PopulateRanges(){
    // bookkeeps the available memory at boot time
    using namespace limine;
    std::uint64_t mem_entries_t = 
      requests::memorymap_request.response->entry_count;
    std::uint8_t rangeIndex = 0;
    limine_memmap_entry** entries = requests::memorymap_request.response->entries;
    for(std::uint64_t i = 0; i < mem_entries_t; i++){
      // only count memory mapping regions which are availabe
      if(entries[i]->type == LIMINE_MEMMAP_USABLE){
        MemoryRange range{
          .lower = entries[i]->base,
          .higher = entries[i]->base + entries[i]->length
        };
        m_memories[rangeIndex++] = range;
      }
    }
    while(rangeIndex < PhysicalMemory::ranges){
      MemoryRange range{
        .lower = 0,
        .higher = 0
      };
      m_memories[rangeIndex++] = range;
    }
  }
  
  void PhysicalMemory::PrintPhysicalMap() const{
    // the physical map is a mapping of the current usable ram
    for(std::size_t i = 0; i < ranges; i++){
      kout << "entry " << i << " :: " 
           << m_memories[i].lower << " :: " << m_memories[i].higher << '\n';
    }
  }

  FreeChunks PhysicalMemory::FindChunks(std::uint8_t index) const{
    // procedure to find buddy chunks of pages
    // policy is first fit so if there is memory at the start which is 2**k, then we allocate 
    // mark it as an 2**k area.
    kout << intmode::dec;
    MemoryRange range{m_memories[index]};
    std::uint64_t available = range.higher - range.lower;
    kout << "current initial range has length: " << available << '\n';
    std::uint8_t order = BuddyAllocator::order;
    FreeChunks chunkies;
    // start from 2^order and go downwards from the input range
    for(int i = order; i >= 0; i--){
      std::uint64_t nchunks = available/(1 << i);
      std::uint16_t chunksize = (1 << i);
      kout << "order :: " << i 
           << " :: chunks of size :: " << chunksize
           << " :: number of chunks :: " << nchunks << '\n';
      if(nchunks > 0){
        chunkies[i] = nchunks;
        available -= (nchunks * chunksize);
      }
      kout << "available afterwards :: " << available << '\n';
    }
    return chunkies;
  }

  void PhysicalMemory::PrintChunkDescriptors(std::uint8_t rangeIdx) const{
    // convenient function to check where the memory chunks of pages begin
    FreeChunks chunkies = FindChunks(rangeIdx);
    paddr64_t base = m_memories[rangeIdx].lower;
    paddr64_t end = m_memories[rangeIdx].higher;
    paddr64_t ptr = base;
    kout << intmode::hex;
    kout << "base = " << base << ", end = " << end << '\n';
    for(int i = BuddyAllocator::order; i >= 0; i--){
      std::uint64_t nchunks = chunkies[i];
      std::uint16_t chunksize = (1 << i);
      if(nchunks == 0){
        continue;
      }
      else{
        for(std::uint64_t i = 0; i < nchunks; i++){
          kout << "address :: " << ptr 
               << " :: has chunk of size :: " << chunksize << '\n';
          ptr += chunksize;
        }
      }
    }
  }

  std::uint64_t PhysicalMemory::GetTopOfPhysicalRam() const{
    std::uint8_t i = 0;
    while(!IsEmptyRange(i)){
      i++;
    }
    return m_memories[--i].higher;
    return 0;
  }

  std::uint64_t PhysicalMemory::GetAvailableMemory() const{
    std::uint64_t availableMemory = 0;
    for(std::uint8_t i = 0; i < ranges; i++){
      if(!IsEmptyRange(i)){
        std::uint64_t length = m_memories[i].higher - m_memories[i].lower;
        availableMemory += length;
      }
    }
    return availableMemory;
  }
}
