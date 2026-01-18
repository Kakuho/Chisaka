#include "frame_array.hpp"
#include "memory/address.hpp"
#include "memory/page/constants.hpp"
#include "palloc.hpp"
#include "debug.hpp"
#include <cstddef>

namespace{
  Mem::Page::FrameArray g_frameArray;
}

namespace Mem::Page{

FrameArray& FrameArray::Get() noexcept{
  return g_frameArray;
}

void FrameArray::Init(const MemoryMapDescriptor& memmap) noexcept{
  Debug::Print("Initialising Page Frame Array...");
  InitMemory(memmap);
  InitFrameDescriptors(memmap);
}

void FrameArray::InitMemory(const MemoryMapDescriptor& memmap) noexcept{
  std::size_t bytes = MemoryRequired(memmap);
  std::size_t pages = bytes / 0x1000;
  if(bytes % 0x1000 != 0){
    pages += 1;
  }
  m_buffer = static_cast<FrameDescriptor*>(Kernel::palloc(pages));
  Debug::Print("Frame Array Bytes Required: ", bytes);
  Debug::Print("Frame Array Pages Required: ", pages);
}

std::size_t FrameArray::MemoryRequired(const MemoryMapDescriptor& memmap) noexcept{
  std::size_t pageIndex = 0;
  for(std::uint8_t i = 0; i < memmap.Entries(); i++){
    const MemoryMapEntry& entry = memmap.Entry(i);
    Chisaka::PhysAddr current = entry.base;
    while(current < entry.base + entry.length){
      pageIndex++;
      current += PAGE_SIZE;
    }
  }
  return pageIndex * PAGE_SIZE;
}

void FrameArray::InitFrameDescriptors(const MemoryMapDescriptor& memmap) noexcept{
  std::size_t pageIndex = 0;
  for(std::uint8_t i = 0; i < memmap.Entries(); i++){
    const MemoryMapEntry& entry = memmap.Entry(i);
    Chisaka::PhysAddr current = entry.base;
    while(current < entry.base + entry.length){
      m_buffer[pageIndex] = FrameDescriptor{current, entry.type};
      current += PAGE_SIZE;
    }
  }
}

std::size_t FrameArray::IndexOf(Chisaka::PhysAddr paddr) const noexcept{
  kassert(paddr < 0x240000000);
  return paddr/PAGE_SIZE;
}

Chisaka::PhysAddr FrameArray::BaseOf(std::size_t index) const noexcept{ 
  kassert(index < m_pages);
  return m_buffer[index].Base();
}

void FrameArray::PrintBases() const noexcept{
  kout << "Printing Page Frame Bases..." << '\n';
  for(std::size_t i = 0; i < m_pages; i++){
    kout << intmode::hex << "Page " << i << " Base :: " << m_buffer[i].Base();
  }
}

FrameDescriptor& FrameArray::operator[](std::size_t index) noexcept{
  return m_buffer[index];
}

const FrameDescriptor& FrameArray::operator[](std::size_t index) const noexcept{
  return m_buffer[index];
}

}
