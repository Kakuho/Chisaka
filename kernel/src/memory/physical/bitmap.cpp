#include "bitmap.hpp"
#include "limine/requests.hpp"

namespace Mem::Phys{

using ByteType = char;
using AddrType = void*;

// ------------------------------------------------------ //
//  Lifetime and initial setup
// ------------------------------------------------------ //

Bitmap::Bitmap() noexcept:
  m_handle{
    nullptr, 
    nullptr
  },
  m_maxIndex{
    PageFrameSizeNBytes() - 1u
  },
  m_lastIndex{0},
  m_totalRegionSize{0}
{
  // I want to place the bookkeeping data in the largest 
  // available memory,
  // so I dont have to worry about running out of page frames
  auto [base, length] = LongestBaseLength();
#if DEBUG
  kout << "base :: " << base << '\n';
  kout << "length :: " << length << '\n';
#endif
  // I use the bump allocator just for initialising the bitmap
  std::size_t useablePages = TotalUseablePageFrames<0x1000>();
#if DEBUG
  kout << "useable Pages :: " << useablePages << '\n';
#endif
  std::size_t totalBytes = PageFrameSizeNBytes() + 
  RegionSizeNBytes();
#if DEBUG
  kout << "Page Frame Bitmap Bytes :: " << PageFrameSizeNBytes() << '\n';
  kout << "Region Array Bytes :: " << RegionSizeNBytes() << '\n';
  kout << "totalBytes :: " << totalBytes << '\n';
#endif
  // setting the handler's base pointers
  m_handle.bitmap = reinterpret_cast<char*>(base);
  //  the offset, if tightly packed, should be 
  //  Ceiling(useablePages/8)
  //
  //  However I changed it to useablePages/8 + 1, which although pads 
  //  it slightly, makes the math easier to implement
  std::size_t offset = (useablePages/8) + 1;
#if DEBUG
  kout << "offset :: " << offset << '\n';
#endif
  InitialiseBitmap(reinterpret_cast<void*>(base), offset);
  InitialiseRegions(reinterpret_cast<Region*>(base + offset));
  Epilogue();
}

void Bitmap::InitialiseRegions(Region* regionBase) noexcept{
  // Regions represents continous areas of useable available memory
  using namespace limine::requests;
  // make space for the region bitmap
  memset(reinterpret_cast<void*>(regionBase), 0, RegionSizeNBytes());
  std::uint32_t pageIndex = 0;    
  std::size_t regionIndex = 0;  // index into the region array
  limine_memmap_response* response = memorymap_request.response;
  for(std::size_t i = 0; i < response->entry_count; i++){
    auto memEntry = response->entries[i];
    if(memEntry->type == LIMINE_MEMMAP_USABLE){
      std::uint32_t relativeIndex = memEntry->length / 0x1000;
      regionBase[regionIndex++] = Region
        {
          .startAddr = memEntry->base,
          .endAddr = memEntry->base + memEntry->length,
          .startIndex = pageIndex,
          .endIndex = pageIndex + relativeIndex
        };
      pageIndex += relativeIndex;
    }
  }
  m_handle.regions = regionBase;
  m_handle.end = 
    reinterpret_cast<void*>(
    reinterpret_cast<std::uint64_t>(regionBase + RegionSizeNBytes())
    );
}

void Bitmap::InitialiseBitmap(void* base, std::size_t length) noexcept{
  // Regions represents continous areas of useable available memory
  memset(reinterpret_cast<void*>(base), 0, length);
  m_handle.bitmap = reinterpret_cast<char*>(base);
}

void Bitmap::Epilogue() noexcept{
  // finishes set up, by setting the used bitmap entries
  physaddr_t start = reinterpret_cast<physaddr_t>(m_handle.bitmap);
  while(start < reinterpret_cast<physaddr_t>(m_handle.end)){
    SetIndex(IndexFrom(reinterpret_cast<void*>(start)));
    start += 0x1000;
  }
}

// ------------------------------------------------------ //
//  Determining number of Bytes required
// ------------------------------------------------------ //

std::size_t Bitmap::RegionSizeNBytes() const noexcept{
  // returns the total size required in bytes for the region array
  using namespace limine::requests;
  if(m_totalRegionSize != 0){
    return m_totalRegionSize;
  }
  std::uint8_t count = 0;
  limine_memmap_response* response = memorymap_request.response;
  for(std::size_t i = 0; i < response->entry_count; i++){
    auto memEntry = response->entries[i];
    if(memEntry->type == LIMINE_MEMMAP_USABLE){
      count++;
    }
  }
  m_totalRegionSize = count * sizeof(Region);
  return m_totalRegionSize;
}

std::size_t Bitmap::PageFrameSizeNBytes() const noexcept{
  // returns the total size required in bytes for page frame statuses 
  // each page frame is a single bit
  return TotalUseablePageFrames<0x1000>() / 8;
}

// ------------------------------------------------------ //
//  Mapping between Physical To Index
// ------------------------------------------------------ //

std::size_t Bitmap::IndexFrom(AddrType paddr) const noexcept{
  // 100 is a invalid return value
  for(std::size_t i = 0; i < m_totalRegionSize; i++){
    if(m_handle.regions[i].ContainsAddr(paddr)){

      std::uint8_t relativeIndex = 
        reinterpret_cast<std::uint64_t>(paddr) - m_handle.regions[i].startAddr / 0x1000;
      return m_handle.regions[i].startIndex + relativeIndex;
    }
  }
  return m_maxIndex;
}

AddrType Bitmap::AddressFrom(std::size_t index) const noexcept{
  for(std::size_t i = 0; i < m_totalRegionSize; i++){
    if(m_handle.regions[i].ContainsIndex(index)){
      std::uint8_t relativeIndex = index - m_handle.regions[i].startIndex;
      return 
        reinterpret_cast<void*>(
          reinterpret_cast<std::uint64_t>(m_handle.regions[i].startAddr) + 0x1000*
            relativeIndex
        );
    }
  }
  return nullptr;
}

// ------------------------------------------------------ //
//  PMM Interface
// ------------------------------------------------------ //

// Helpers for page alloc and page free

void Bitmap::SetIndex(std::size_t index) noexcept{
  // input:   individual page index
  // effect:  sets the bitmap page index to 1
  std::size_t pageIndex = index / 8;
  std::uint8_t relativeIndex = index % 8;
  m_handle.bitmap[pageIndex] |= (1 << relativeIndex);
}

void Bitmap::ClearIndex(std::size_t index) noexcept{
  std::size_t pageIndex = index / 8;
  std::uint8_t relativeIndex = index % 8;
  m_handle.bitmap[pageIndex] &= ~(1 << relativeIndex);
}

std::uint8_t Bitmap::GetFreeIndex(std::uint8_t byte) const noexcept{
  std::uint8_t index = 0;
  std::uint8_t mask = ~(1 << index);
  while((byte & mask) != 0){
    index++;
    mask = ~(1 << index);
  }
  return index;
}

// the actual interface themselves

AddrType Bitmap::AllocPage() noexcept{
  // returns a nullptr on invalid calls
  if(m_useableIndicies != 0){
    std::size_t byteIndex = m_lastIndex;
    bool isFree = static_cast<std::uint8_t>(m_handle.bitmap[byteIndex]) != 0xFF;
    while(!isFree){
      byteIndex = (byteIndex + 1) % m_maxIndex;
    }
    std::uint8_t freeIndex = GetFreeIndex(m_handle.bitmap[byteIndex]);
    SetIndex(freeIndex);
    return AddressFrom(freeIndex);
  }
  else{
    return nullptr;
  }
}

void Bitmap::FreePage(AddrType paddr) noexcept{
  ClearIndex(IndexFrom(paddr));
}

} // namespace Mem::Phys
