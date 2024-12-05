#ifndef DRIVERS_SATA_FIS_DMA_ACTIVATE_HPP
#define DRIVERS_SATA_FIS_DMA_ACTIVATE_HPP

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

#include "drivers/sata/fis/args.hpp"
#include "fis_types.hpp"

namespace Drivers::Sata::Fis::DmaActivate{

struct Initialiser{
  Args::PortMultiplier portMultiplier;
};

class Frame{
  static constexpr std::uint8_t TYPE_VALUE = 
    static_cast<std::uint8_t>(FisType::DMAActivate);

  public:
    constexpr Frame(Initialiser&& src) noexcept;

    [[nodiscard]] constexpr std::uint8_t Type() const noexcept{
      return m_type;
    }

    [[nodiscard]] constexpr std::uint8_t PortMultiplier() const noexcept{
      return m_portMultiplier;
    }

  private:
    // Dword 0
    std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_portMultiplier;
    [[maybe_unused]] std::uint16_t m_reserved = 0;
};

static_assert(sizeof(Frame) == 4);

constexpr Frame::Frame(Initialiser&& src) noexcept
  : m_portMultiplier{src.portMultiplier.data}
{

}

}

#endif
