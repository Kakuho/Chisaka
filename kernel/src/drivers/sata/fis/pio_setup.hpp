#ifndef DRIVERS_SATA_FIS_PIO_SETUP_HPP
#define DRIVERS_SATA_FIS_PIO_SETUP_HPP

//  Encapsulates PIO Setup FISes (D2H)
//

#include <cstdint>

#include "fis_types.hpp"
#include "args.hpp"

namespace Drivers::Sata::Fis::PioSetup{

class Frame{
  static constexpr std::uint8_t TYPE_VALUE = 
    GetUnderlying(FisType::PIOSetup);

  public:
    Frame() = default;

    // ------------------------------------------------------ //
    //  Attribute Querying
    // ------------------------------------------------------ //

    [[nodiscard]] constexpr std::uint8_t Type() const noexcept;
    [[nodiscard]] constexpr std::uint8_t PortSelector() const noexcept;
    [[nodiscard]] constexpr bool Interrupt() const noexcept;
    [[nodiscard]] constexpr std::uint8_t Status() const noexcept;
    [[nodiscard]] constexpr std::uint8_t Error() const noexcept;
    [[nodiscard]] constexpr std::uint8_t Device() const noexcept;
    [[nodiscard]] constexpr std::uint16_t SectorCount() const noexcept;

    [[nodiscard]] constexpr std::uint16_t LbaLow() const noexcept;
    [[nodiscard]] constexpr std::uint16_t LbaMid() const noexcept;
    [[nodiscard]] constexpr std::uint16_t LbaHigh() const noexcept;

  public:
    // public for now, but i will change its interface later on
    
    // Dword 0
    const std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_i_d_portMultiplier;
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
    [[maybe_unused]] std::uint8_t m_rsv0 = 0;
    // Dword 3
    std::uint16_t m_sectorCountReg;
    [[maybe_unused]] std::uint8_t m_rsv1 = 0;
    std::uint8_t m_e_status;
    // Dword 4
    std::uint16_t m_transferCount;
    [[maybe_unused]] std::uint16_t m_rsv2 = 0;
};

static_assert(sizeof(Frame) == 20);

// Implementation

[[nodiscard]] constexpr std::uint8_t Frame::Type() const 
noexcept{
  return m_type;
}

[[nodiscard]] constexpr 
std::uint8_t Frame::PortSelector() const noexcept{
  return m_i_d_portMultiplier & 0x0F;
}

[[nodiscard]] constexpr 
bool Frame::Interrupt() const noexcept{
  return m_i_d_portMultiplier & 0x40;
}

[[nodiscard]] constexpr 
std::uint8_t Frame::Status() const noexcept{
  return m_statusReg;
}

[[nodiscard]] constexpr 
std::uint8_t Frame::Error() const noexcept{
  return m_errorReg;
}

[[nodiscard]] constexpr 
std::uint8_t Frame::Device() const noexcept{
  return m_deviceReg;
}

[[nodiscard]] constexpr 
std::uint16_t Frame::SectorCount() const noexcept{
  return m_sectorCountReg;
}

[[nodiscard]] constexpr 
std::uint16_t Frame::LbaLow() const noexcept{
  return (m_lbaLowReg1 << 8) | m_lbaLowReg0;
}

[[nodiscard]] constexpr 
std::uint16_t Frame::LbaMid() const noexcept{
  return (m_lbaMidReg1 << 8) | m_lbaMidReg0;
}

[[nodiscard]] constexpr 
std::uint16_t Frame::LbaHigh() const noexcept{
  return (m_lbaHighReg1 << 8) | m_lbaHighReg0;
}

}

#endif
