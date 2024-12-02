#ifndef DRIVERS_SATA_FIS_H2D_REGISTER_FRAME_HPP
#define DRIVERS_SATA_FIS_H2D_REGISTER_FRAME_HPP

//  Encapsulates Register - Host to Device Fises
//
//  These FISes are used by the host to issue ATA commands to a device. 
//  This is by sending the shadow register block to the device.
// 
//  Upon reception, the device updates its registers with the shadow 
//  block registers in the FIS. The device can then either:
//    
//    a) Executes a command supplied in the Command Register
//
//    b) Executes a control request supplied in the Device Control 
//       Register

#include <cstdint>

#include "fis_types.hpp"
#include "args.hpp"

namespace Drivers::Sata{

struct H2DRegisterFrameArgs{
  Args::C               isC;
  Args::PortMultiplier  portMultiplier;
  Args::Command         command;
  Args::Features        features;
  Args::LbaLow          lbaLow;
  Args::LbaMid          lbaMid;
  Args::LbaHigh         lbaHigh;
  Args::Device          device;
  Args::SectorCount     sectorCount;
  Args::Control         control;
};

class H2DRegisterFrame{
  static constexpr std::uint8_t TYPE_VALUE = 
    static_cast<std::uint8_t>(FisType::RegisterHostToDevice);

  public:
    H2DRegisterFrame();
    H2DRegisterFrame(H2DRegisterFrameArgs&& src);

    //-------------------------------------------------------------
    //  Queries
    //-------------------------------------------------------------

    [[nodiscard]] constexpr std::uint8_t TypeValue() const noexcept;
    [[nodiscard]] constexpr bool IsC() const noexcept;
    [[nodiscard]] constexpr std::uint8_t SelectedPort() const noexcept;
    [[nodiscard]] constexpr std::uint8_t Command() const noexcept;
    [[nodiscard]] constexpr std::uint16_t Features() const noexcept;
    [[nodiscard]] constexpr std::uint16_t LbaLow() const noexcept;
    [[nodiscard]] constexpr std::uint16_t LbaMid() const noexcept;
    [[nodiscard]] constexpr std::uint16_t LbaHigh() const noexcept;
    [[nodiscard]] constexpr std::uint8_t Device() const noexcept;
    [[nodiscard]] constexpr std::uint16_t SectorCount() const noexcept;
    [[nodiscard]] constexpr std::uint8_t Control() const noexcept;

  private:
    // used by the constructor to split up ctor arguments

    constexpr void SplitFeatures(std::uint16_t features) noexcept;
    constexpr void SplitLbaLow(std::uint16_t lbaLow) noexcept;
    constexpr void SplitLbaMid(std::uint16_t lbaMid) noexcept;
    constexpr void SplitLbaHigh(std::uint16_t lbaHigh) noexcept;

    constexpr void PackCPortmultiplier(
      bool c, 
      std::uint8_t portMultiplier
    ) 
    noexcept;

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
    [[maybe_unused]] std::uint8_t m_rsv0 = 0;
    std::uint8_t m_controlReg;
    // Dword 4
    [[maybe_unused]] std::uint32_t m_rsv1 = 0;
};

static_assert(sizeof(H2DRegisterFrame) == 20);


// ------------------------------------------------------ //
//  Queries
// ------------------------------------------------------ //

[[nodiscard]] constexpr 
std::uint8_t H2DRegisterFrame::TypeValue() const noexcept{
  return m_type;
}

[[nodiscard]] constexpr bool H2DRegisterFrame::IsC() const noexcept{
  return m_c_portMultiplier & 0x80;
}

[[nodiscard]] constexpr 
std::uint8_t H2DRegisterFrame::SelectedPort() const noexcept{
  return m_c_portMultiplier & 0x0F;
}

[[nodiscard]] constexpr
std::uint8_t H2DRegisterFrame::Command() const noexcept{
  return m_commandReg;
}

[[nodiscard]] constexpr
std::uint16_t H2DRegisterFrame::Features() const noexcept{
  return (m_featuresReg1 << 8) | m_featuresReg0;
}

[[nodiscard]] constexpr
std::uint16_t H2DRegisterFrame::LbaLow() const noexcept{
  return (m_lbaLowReg1 << 8) | m_lbaLowReg0;
}

[[nodiscard]] constexpr
std::uint16_t H2DRegisterFrame::LbaMid() const noexcept{
  return (m_lbaMidReg1 << 8) | m_lbaMidReg0;
}

[[nodiscard]] constexpr
std::uint16_t H2DRegisterFrame::LbaHigh() const noexcept{
  return (m_lbaHighReg1 << 8) | m_lbaHighReg0;
}

[[nodiscard]] constexpr
std::uint8_t H2DRegisterFrame::Device() const noexcept{
  return m_deviceReg;
}

[[nodiscard]] constexpr
std::uint16_t H2DRegisterFrame::SectorCount() const noexcept{
  return m_sectorCountReg;
}

[[nodiscard]] constexpr
std::uint8_t H2DRegisterFrame::Control() const noexcept{
  return m_controlReg;
}

// ------------------------------------------------------ //
//  Private Helper Routines
// ------------------------------------------------------ //

constexpr void H2DRegisterFrame::SplitFeatures(std::uint16_t features) 
noexcept{
  m_featuresReg1 = (features & 0xF0) >> 8;
  m_featuresReg0 = features & 0x0F;
}

constexpr void H2DRegisterFrame::SplitLbaLow(std::uint16_t lbaLow) 
noexcept{
  m_lbaLowReg1 = (lbaLow & 0xF0) >> 8;
  m_lbaLowReg0 = lbaLow & 0x0F;
}

constexpr void H2DRegisterFrame::SplitLbaMid(std::uint16_t lbaMid) 
noexcept{
  m_lbaMidReg1 = (lbaMid & 0xF0) >> 8;
  m_lbaMidReg0 = lbaMid & 0x0F;
}

constexpr void H2DRegisterFrame::SplitLbaHigh(std::uint16_t lbaHigh) 
noexcept{
  m_lbaHighReg1 = (lbaHigh & 0xF0) >> 8;
  m_lbaHighReg0 = lbaHigh & 0x0F;
}

constexpr void H2DRegisterFrame::PackCPortmultiplier(
  bool c, 
  std::uint8_t portMultiplier
) 
noexcept{
  if(c){
    m_c_portMultiplier |= 0x80 | (portMultiplier & 0xF);
  }
  else{
    m_c_portMultiplier |= portMultiplier & 0xF;
  }
}

}

#endif
