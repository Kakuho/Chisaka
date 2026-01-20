#pragma once

// Common place to store FIS Type Values

#include <cstdint>

namespace Chisaka::Ahci{

// Reference Serial Ata 10.3

enum class FisType: std::uint8_t{
  RegisterHostToDevice = 0x27,
  RegisterDeviceToHost = 0x34,
  DMAActivate = 0x39,
  DMASetup = 0x41,
  Data = 0x46,
  BISTActivate = 0x58,
  PIOSetup = 0x5f,
  SetDeviceBits = 0xA1,
};

constexpr std::uint8_t GetUnderlying(FisType type){
  return static_cast<std::uint8_t>(type);
}

}
