#ifndef MEMORY_MAP_ENTRY_HPP
#define MEMORY_MAP_ENTRY_HPP

#include <cstdint>

namespace Mem{

struct MemoryMapEntry{

  enum class Type: std::uint8_t{
    Useable,
    Reserved,
    AcpiReclaimable,
    AcpiNVS,
    BadMemory,
    BootLoaderReclaimable,
    Kernel,
    FrameBuffer,
    Empty,                    // used as the default empty value
    Unknown                   // used during code decoding
  };

  static constexpr Type ConvertLimineType(std::uint64_t limineCode) noexcept{
    using enum Type;
    switch(limineCode){
      case 0:
        return Useable;
      case 1:
        return Reserved;
      case 2:
        return AcpiReclaimable;
      case 3:
        return AcpiNVS;
      case 4:
        return BadMemory;
      case 5:
        return BootLoaderReclaimable;
      case 6:
        return Kernel;
      case 7:
        return FrameBuffer;
      default:
        return Unknown;
    }
  }

  static constexpr const char* MemTypeToString(Type type) noexcept{
    using enum Type;
    switch(type){
      case Useable:
        return "Useable";
      case Reserved:
        return "Reserved";
      case AcpiReclaimable:
        return "AcpiReclaimable";
      case AcpiNVS:
        return "AcpiNVS";
      case BadMemory:
        return "BadMemory";
      case BootLoaderReclaimable:
        return "BootLoaderReclaimable";
      case Kernel:
        return "Kernel";
      case FrameBuffer:
        return "FrameBuffer";
      case Empty:
        return "Empty";
      case Unknown:
        [[fallthrough]];
      default:
        return "Unknown";
    }
  }

  std::uint64_t base;
  std::uint64_t length;
  Type type;
};

// wrapper functions 

constexpr MemoryMapEntry::Type ConvertLimineType(
    std::uint64_t limineCode
  ) noexcept
{
  return MemoryMapEntry::ConvertLimineType(limineCode);
}

constexpr const char* MemTypeToString(MemoryMapEntry::Type type) noexcept{
  return MemoryMapEntry::MemTypeToString(type);
}

} // namespace Mem

#endif
