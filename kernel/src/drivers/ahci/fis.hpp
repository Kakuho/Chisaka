#ifndef DRIVERS_AHCI_FIS_HPP
#define DRIVERS_AHCI_FIS_HPP

#include <cstdint>

namespace Drivers::Ahci::Fis{
  // Reference Serial Ata 10.3

  enum class Type: std::uint8_t{
    RegisterHostToDevice = 0x27,
    RegisterDeviceToHost = 0x34,
    DMAActivate = 0x39,
    DMASetup = 0x41,
    Data = 0x46,
    BISTActivate = 0x58,
    PIOSetup = 0x5f,
    SetDeviceBits = 0xA1,
  };

  // Below contains the actual FIS Definitions

  // used to transfer information from the shadow registers to the device
  struct H2DRegister{
    Type fisType = Type::RegisterHostToDevice;
    std::uint8_t portMultiplier = 0;
    std::uint8_t command;
    std::uint8_t features0;

    std::uint8_t lbaLow0;
    std::uint8_t lbaMid0;
    std::uint8_t lbaHigh0;
    std::uint8_t device;

    std::uint8_t lbaLow1;
    std::uint8_t lbaMid1;
    std::uint8_t lbaHigh1;
    std::uint8_t features1;
    
    std::uint16_t sectorCount;
    std::uint8_t rsv0 = 0;
    std::uint8_t control;

    std::uint32_t rsv1 = 0;
  };
  static_assert(sizeof(H2DRegister) == 20);

  // usually used to indicate command completion
  struct D2HRegister{
    Type fisType;
    std::uint8_t i_portMultiplier;
    std::uint8_t status;
    std::uint8_t error;

    std::uint8_t lbaLow0;
    std::uint8_t lbaMid0;
    std::uint8_t lbaHigh0;
    std::uint8_t device;

    std::uint8_t lbaLow1;
    std::uint8_t lbaMid1;
    std::uint8_t lbaHigh1;
    std::uint8_t rsv0 = 0;
    
    std::uint16_t sectorCount;
    std::uint16_t rsv1 = 0;

    std::uint32_t rsv2 = 0;
  };
  static_assert(sizeof(D2HRegister) == 20);
}

#endif
