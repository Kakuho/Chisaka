#pragma once

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
    switch(limineCode){
      case 0:
        return Type::Useable;
      case 1:
        return Type::Reserved;
      case 2:
        return Type::AcpiReclaimable;
      case 3:
        return Type::AcpiNVS;
      case 4:
        return Type::BadMemory;
      case 5:
        return Type::BootLoaderReclaimable;
      case 6:
        return Type::Kernel;
      case 7:
        return Type::FrameBuffer;
      default:
        return Type::Unknown;
    }
  }

  static constexpr const char* MemTypeToString(Type type) noexcept{
    switch(type){
      case Type::Useable:
        return "Useable";
      case Type::Reserved:
        return "Reserved";
      case Type::AcpiReclaimable:
        return "AcpiReclaimable";
      case Type::AcpiNVS:
        return "AcpiNVS";
      case Type::BadMemory:
        return "BadMemory";
      case Type::BootLoaderReclaimable:
        return "BootLoaderReclaimable";
      case Type::Kernel:
        return "Kernel";
      case Type::FrameBuffer:
        return "FrameBuffer";
      case Type::Empty:
        return "Empty";
      case Type::Unknown:
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
