#ifndef DRIVERS_SATA_FIS_SET_DEVICE_BITS_FRAME_HPP
#define DRIVERS_SATA_FIS_SET_DEVICE_BITS_FRAME_HPP

//  Encapsulates DMA Activate - Device to Host FISes
//
//  Used by the Device to indicate a host can carry on with DMA data 
//  transfer operations.
//
//  In the situation where a Host needs to send multiple Data FISes, 
//  the Host must wait for successful reception of a DMA Activate FIS
//  before they can carry on with sending the next Data FIS.

#include <cstdint>
#include <cassert>

#include "fis_types.hpp"

namespace Drivers::Sata{

class DmaActivate{
  static constexpr std::uint8_t TYPE_VALUE = 
    static_cast<std::uint8_t>(FisType::DMAActivate);

  public:
    DmaActivate() = default;

  private:
    // Dword 0
    std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_portMultiplier;
    std::uint16_t m_reserved;
};

static_assert(sizeof(DmaActivate) == 4);

}

#endif
