#include "bitmap.hpp"
#include "limine/requests.hpp"

namespace Mem::Phys{

Bitmap::Bitmap() noexcept{
  // I want to place the internal data in the largest available memory,
  // so I dont have to worry about running out of space
  std::uint64_t largestBase = 0;
  std::uint64_t length = 0;
  {
    using namespace limine::requests;
    // setting up
    if(memorymap_request.response == nullptr){
      kout << "MEM FAIL\n";
    }
    limine_memmap_response* response = memorymap_request.response;
    std::uint64_t totalEntries = response->entry_count;
    limine_memmap_entry** entries = response->entries;
    // now we can begin looping through to get largest available 
    // region
    std::uint64_t largestIndex = 0;
    for(std::uint64_t i = 0; i < totalEntries; i++){
      // now set the responses
      if(entries[i] == LIMINE_MEMMAP_USABLE){
        if(largestIndex == 0){
          largestIndex = i;
        }
        else{
          if(entries[i]->length > entries[largestIndex]->length){
            largestIndex = i;
          }
        }
      }
    }
    largestBase = entries[largestIndex]->base;
    length = entries[largestIndex]->length;
  }
  // now we can begin setting up the Bump allocator
  Bump boostrapper{largestBase, largestBase + length};
}

} // namespace Mem::Phys
