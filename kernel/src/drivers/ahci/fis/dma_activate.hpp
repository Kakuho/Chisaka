#pragma once

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

namespace Chisaka::Ahci{

class DmaActivateFis{
  static constexpr std::uint8_t TYPE_VALUE = 
    GetUnderlying(FisType::DMAActivate);

  public:
    constexpr DmaActivateFis() noexcept;

    constexpr std::uint8_t Type() const noexcept{ return m_type;}
    constexpr std::uint8_t PortMultiplier() const noexcept{ return m_portMultiplier;}

  private:
    // Dword 0
    const std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_portMultiplier;
    [[maybe_unused]] std::uint16_t m_reserved = 0;
};

static_assert(sizeof(DmaActivateFis) == 4);

//-----------------------------------------------------------------------------------------

constexpr DmaActivateFis::DmaActivateFis() noexcept
  :
    m_portMultiplier{0}
{

}

}
