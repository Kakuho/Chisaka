#ifndef DRIVERS_SATA_FIS_D2H_REGISTER_FRAME_HPP
#define DRIVERS_SATA_FIS_D2H_REGISTER_FRAME_HPP

//  Encapsulates Register - Device to Host FISes
//
//  These FISes are used by the device to notify the host that a command 
//  has completed, or to change the shadow block registers of the hba 
//  otherwise.
//
//  Upon reception, the hba's shadow block register are updated with the 
//  contents of the FIS.

#include <cstdint>
#include "fis_types.hpp"

namespace Drivers::Sata{

class D2HRegisterFrame{
  static constexpr std::uint8_t TYPE_VALUE = 
    static_cast<std::uint8_t>(FisType::RegisterDeviceToHost);

  public:
    D2HRegisterFrame() = default;

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
    [[maybe_unused]] std::uint8_t m_rsv0 = 0;
    // Dword 3
    std::uint16_t m_sectorCountReg;
    [[maybe_unused]] std::uint16_t m_rsv1 = 0;
    // Dword 4
    [[maybe_unused]] std::uint32_t m_rsv2 = 0;
};

static_assert(sizeof(D2HRegisterFrame) == 20);

// Implementation

[[nodiscard]] constexpr std::uint8_t D2HRegisterFrame::Type() const 
noexcept{
  return m_type;
}

[[nodiscard]] constexpr 
std::uint8_t D2HRegisterFrame::PortSelector() const noexcept{
  return m_i_portMultiplier & 0x0F;
}

[[nodiscard]] constexpr 
bool D2HRegisterFrame::Interrupt() const noexcept{
  return m_i_portMultiplier & 0x40;
}

[[nodiscard]] constexpr 
std::uint8_t D2HRegisterFrame::Status() const noexcept{
  return m_statusReg;
}

[[nodiscard]] constexpr 
std::uint8_t D2HRegisterFrame::Error() const noexcept{
  return m_errorReg;
}

[[nodiscard]] constexpr 
std::uint8_t D2HRegisterFrame::Device() const noexcept{
  return m_deviceReg;
}

[[nodiscard]] constexpr 
std::uint16_t D2HRegisterFrame::SectorCount() const noexcept{
  return m_sectorCountReg;
}

[[nodiscard]] constexpr 
std::uint16_t D2HRegisterFrame::LbaLow() const noexcept{
  return (m_lbaLowReg1 << 8) | m_lbaLowReg0;
}

[[nodiscard]] constexpr 
std::uint16_t D2HRegisterFrame::LbaMid() const noexcept{
  return (m_lbaMidReg1 << 8) | m_lbaMidReg0;
}

[[nodiscard]] constexpr 
std::uint16_t D2HRegisterFrame::LbaHigh() const noexcept{
  return (m_lbaHighReg1 << 8) | m_lbaHighReg0;
}

}

#endif
