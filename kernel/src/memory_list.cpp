/*
#include "memory_list.hpp"
#include <cstdint>

namespace memory{

memlist::memlist(){
  if(memorymap_request.response == nullptr){
    while(true){
    }
  }
  else{
    std::uint64_t mem_entries = memorymap_request.response->entry_count;
    for(auto i{0}; i < mem_entries; i++){
      if(memorymap_request.response->entries[i]->type == 0){
        // memory block is usable, add it to the memory tree
      }
    }
  }
}

}
*/
