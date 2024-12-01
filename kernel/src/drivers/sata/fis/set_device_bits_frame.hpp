#ifndef DRIVERS_SATA_FIS_SET_DEVICE_BITS_FRAME_HPP
#define DRIVERS_SATA_FIS_SET_DEVICE_BITS_FRAME_HPP

//  Encapsulates Set Device Bits - Device to Host FISes
//
//  These FISes are used by the device to set the shadow block registers
//  which cannot otherwise be obtained, notably parts of the Error 
//  Register and the Status Register

#include <cmath>
#include <cstdint>
#include "fis_types.hpp"

namespace Drivers::Sata{

class SetDeviceBitsFrame{
  static constexpr std::uint8_t TYPE_VALUE = 
    static_cast<std::uint8_t>(FisType::SetDeviceBits);

  public:
    SetDeviceBitsFrame() = default;

  private:
    // Dword 0
    std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_n_i_portMultiplier;
    std::uint8_t m_statusReg;
    std::uint8_t m_errorReg;
};

}

#endif
