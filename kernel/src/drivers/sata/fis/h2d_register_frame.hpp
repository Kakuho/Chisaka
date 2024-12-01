#ifndef DRIVERS_SATA_FIS_H2D_REGISTER_FRAME_HPP
#define DRIVERS_SATA_FIS_H2D_REGISTER_FRAME_HPP

//  Encapsulates Register - Host to Device Fises
//
//  These Fises are used by the host to issue ATA commands to a device. This is by 
//  sending the shadow register block to the device.
// 
//  Upon reception, the device updates its registers with the shadow block registers in
//  the FIS. The device can then either:
//    
//    a) Executes a command supplied in the Command Register
//
//    b) Executes a control request supplied in the Device Control Register

#include <cstdint>

#include "fis_types.hpp"

namespace Drivers::Sata{

class H2DRegisterFrame{
  static constexpr std::uint8_t TYPE_VALUE = 
    static_cast<std::uint8_t>(FisType::RegisterHostToDevice);

  public:
    H2DRegisterFrame() = default;

    //-------------------------------------------------------------
    //  Queries for the Frame
    //-------------------------------------------------------------

    bool IsC() const noexcept;
    std::uint8_t SelectedPort() const noexcept;
    std::uint8_t Command() const noexcept;
    std::uint8_t Features() const noexcept;
    std::uint8_t LbaLow() const noexcept;
    std::uint8_t LbaMid() const noexcept;
    std::uint8_t LbaHigh() const noexcept;

  private:
    // Dword 0
    std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_c_portMultiplier = 0;
    std::uint8_t m_commandReg;
    std::uint8_t m_featuresReg0;
    // Dword 1
    std::uint8_t m_lbaLowReg0;
    std::uint8_t m_lbaMidReg0;
    std::uint8_t m_lbaHighReg0;
    std::uint8_t m_deviceReg;
    // Dword 2
    std::uint8_t m_lbaLowReg1;
    std::uint8_t m_lbaMidReg1;
    std::uint8_t m_lbaHighReg1;
    std::uint8_t m_featuresReg1;
    // Dword 3
    std::uint16_t m_sectorCountReg;
    std::uint8_t m_rsv0 = 0;
    std::uint8_t m_controlReg;
    // Dword 4
    std::uint32_t m_rsv1 = 0;
};

static_assert(sizeof(H2DRegisterFrame) == 20);

}

#endif
