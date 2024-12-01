#ifndef DRIVERS_SATA_FIS_D2H_REGISTER_FRAME_HPP
#define DRIVERS_SATA_FIS_D2H_REGISTER_FRAME_HPP

//  Encapsulates Register - Device to Host FISes
//
//  These FISes are used by the device to notify the host that a command has 
//  completed, or to change the shadow block registers of the hba otherwise.
//
//  Upon reception, the hba's shadow block register are updated with the contents 
//  of the FIS.

#include <cstdint>
#include "fis_types.hpp"

namespace Drivers::Sata{

class D2HRegisterFrame{
  static constexpr std::uint8_t TYPE_VALUE = 
    static_cast<std::uint8_t>(FisType::RegisterDeviceToHost);


  public:
    D2HRegisterFrame() = default;

    // interface...
    
  private:
    // Dword 0
    std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_i_portMultiplier;
    std::uint8_t m_statusReg;
    std::uint8_t m_errorReg;
    // Dword 1
    std::uint8_t m_lbaLowReg0;
    std::uint8_t m_lbaMidReg0;
    std::uint8_t m_lbaHighReg0;
    std::uint8_t m_deviceReg;
    // Dword 2
    std::uint8_t m_lbaLowReg1;
    std::uint8_t m_lbaMidReg1;
    std::uint8_t m_lbaHighReg1;
    std::uint8_t m_rsv0 = 0;
    // Dword 3
    std::uint16_t m_sectorCountReg;
    std::uint16_t m_rsv1 = 0;
    // Dword 4
    std::uint32_t m_rsv2 = 0;
};

static_assert(sizeof(D2HRegisterFrame) == 20);

}

#endif
