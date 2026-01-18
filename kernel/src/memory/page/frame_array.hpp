#pragma once

// Frame Array keeps global information regarding all the page frames within 
// the system

#include <cstdint>
#include "memory/address.hpp"
#include "memory/memory_map/memory_map_descriptor.hpp"
#include "drivers/serial/kostream.hpp"

#include "frame_descriptor.hpp"
#include "constants.hpp"
#include "types.hpp"

namespace Mem::Page{

class FrameArray{
  public:
    FrameArray() = default;
    FrameArray& Get() noexcept;
    void Init(const MemoryMapDescriptor& memmap) noexcept;

    constexpr std::size_t Size() noexcept {return m_pages;}
    constexpr std::size_t Bytes() noexcept {return Size() * PAGE_SIZE;}

    std::size_t IndexOf(Chisaka::PhysAddr paddr) const noexcept;
    Chisaka::PhysAddr BaseOf(std::size_t index) const noexcept;

    void PrintBases() const noexcept;
      
    FrameDescriptor& operator[](std::size_t index) noexcept;
    const FrameDescriptor& operator[](std::size_t index) const noexcept;

  private:
    std::size_t MemoryRequired(const MemoryMapDescriptor& memmap) noexcept;
    void InitMemory(const MemoryMapDescriptor& memmap) noexcept;
    void InitFrameDescriptors(const MemoryMapDescriptor& memmap) noexcept;

  private:
    FrameDescriptor* m_buffer;
    std::size_t m_pages;
};

}
