#pragma once

// Frame Array keeps global information regarding all the page frames within 
// the system

#include <cstdint>

#include "frame_descriptor.hpp"

#include "types.hpp"

#include "debug.hpp"

#include "memmap/memory_map.hpp"
#include "drivers/serial/kostream.hpp"

namespace Chisaka{

class FrameArray{
  public:
    FrameArray() = default;
    void Init(const MemoryMap& memmap) noexcept;

    constexpr std::size_t Size() noexcept {return m_pages;}
    constexpr std::size_t Bytes() noexcept;

    std::size_t IndexOf(Chisaka::PhysAddr paddr) const noexcept;
    Chisaka::PhysAddr BaseOf(std::size_t index) const noexcept;

    void PrintBases() const noexcept;
      
    FrameDescriptor& operator[](std::size_t index) noexcept;
    const FrameDescriptor& operator[](std::size_t index) const noexcept;

  private:
    std::size_t MemoryRequired(const MemoryMap& memmap) noexcept;
    void InitMemory(const MemoryMap& memmap) noexcept;
    void InitFrameDescriptors(const MemoryMap& memmap) noexcept;

  private:
    FrameDescriptor* m_buffer;
    std::size_t m_pages;
};

}
