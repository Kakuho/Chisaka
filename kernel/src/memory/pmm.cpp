#include "pmm.hpp"

namespace mem{

pmm::pmm(){
  limine_memmap_entry** entries = req::memorymap_request.response->entries;
  std::uint64_t mem_entry_t = req::memorymap_request.response->entry_count;
  for(std::size_t i = 0; i < mem_entry_t; i++){
    if(entries[i]->type == LIMINE_MEMMAP_USABLE){
      freeram += entries[i]->length;
    }
  }
  kout << "pmm initialised::freeram::" << freeram << "::usedram::" << usedram 
       <<'\n';
  kout << "total physical page frames: " << freeram / 0x1000 << '\n';
}

} // namespace mem
