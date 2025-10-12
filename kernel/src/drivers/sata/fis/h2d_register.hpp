#ifndef DRIVERS_SATA_FIS_H2D_REGISTER_HPP
#define DRIVERS_SATA_FIS_H2D_REGISTER_HPP

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
//
//  TODO:
//    
//    * Define better abstraction

#include <cstdint>

#include "fis_types.hpp"
#include "args.hpp"
#include "lib.hpp"

namespace Drivers::Sata::Fis::H2DRegister{

struct Initialiser{
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

class Frame{
  static constexpr std::uint8_t TYPE_VALUE = 
    GetUnderlying(FisType::RegisterHostToDevice);

  public:
    constexpr Frame(Initialiser&& src) noexcept;
    constexpr Frame();

    static Frame CreateWriteFrame(std::uint64_t sector_address){
      static constexpr std::uint8_t DMA_WRITE_EXT_CODE = 0x35;
      auto addresses = ExtractAddress(sector_address);

      Sata::Fis::H2DRegister::Frame dmaWriteFis{};
      kassert(dmaWriteFis.m_type == 0x27);
      dmaWriteFis.m_sectorCountReg = 1;
      // LBA
      dmaWriteFis.m_lbaLowReg0    = std::get<2>(addresses) & 0xFF;
      dmaWriteFis.m_lbaLowReg1    = std::get<2>(addresses) >> 8;
      dmaWriteFis.m_lbaMidReg0    = std::get<1>(addresses) & 0xFF;
      dmaWriteFis.m_lbaMidReg1    = std::get<1>(addresses) >> 8;
      dmaWriteFis.m_lbaHighReg0   = std::get<0>(addresses) & 0xFF;
      dmaWriteFis.m_lbaHighReg1   = std::get<0>(addresses) >> 8;
      // Rest of the features
      dmaWriteFis.m_deviceReg = 0x40;
      dmaWriteFis.m_c_portMultiplier = 0x80;
      dmaWriteFis.m_commandReg = DMA_WRITE_EXT_CODE;
      return dmaWriteFis;
    }

    static Frame CreateReadFrame(std::uint64_t sector_address){
      static constexpr std::uint8_t DMA_READ_EXT_CODE = 0x25;
      auto addresses = ExtractAddress(sector_address);

      Sata::Fis::H2DRegister::Frame dmaReadFis{};
      kassert(dmaReadFis.m_type == 0x27);
      dmaReadFis.m_sectorCountReg = 1;
      // LBA
      dmaReadFis.m_lbaLowReg0    = std::get<2>(addresses) & 0xFF;
      dmaReadFis.m_lbaLowReg1    = std::get<2>(addresses) >> 8;
      dmaReadFis.m_lbaMidReg0    = std::get<1>(addresses) & 0xFF;
      dmaReadFis.m_lbaMidReg1    = std::get<1>(addresses) >> 8;
      dmaReadFis.m_lbaHighReg0   = std::get<0>(addresses) & 0xFF;
      dmaReadFis.m_lbaHighReg1   = std::get<0>(addresses) >> 8;
      // Rest of the features
      dmaReadFis.m_deviceReg = 0x40;
      dmaReadFis.m_c_portMultiplier = 0x80;
      dmaReadFis.m_commandReg = DMA_READ_EXT_CODE;
      return dmaReadFis;
    }
    

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

  public:
    // public for now, will change the interface later on...

    // Dword 0
    const std::uint8_t m_type = TYPE_VALUE;
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

static_assert(sizeof(Frame) == 20);

// ------------------------------------------------------ //
//  Impl
// ------------------------------------------------------ //

constexpr Frame::Frame()
  :
    // Dword 0
    m_c_portMultiplier{0},
    m_commandReg{0},
    m_featuresReg0{0},
    // Dword 1
    m_lbaLowReg0{0},
    m_lbaMidReg0{0},
    m_lbaHighReg0{0},
    m_deviceReg{0},
    // Dword 2
    m_lbaLowReg1{0},
    m_lbaMidReg1{0},
    m_lbaHighReg1{0},
    m_featuresReg1{0},
    // Dword 3
    m_sectorCountReg{0},
    m_controlReg{0}
{

}

constexpr 
Frame::Frame(Initialiser&& src)
  noexcept:
    m_commandReg{src.command.data},
    m_deviceReg{src.device.data},
    m_sectorCountReg{src.sectorCount.data},
    m_controlReg{src.control.data}
{
  m_c_portMultiplier = src.portMultiplier.data | (src.isC.data 
    ? 0x80 : 0x00);
  std::tie(m_featuresReg0, m_featuresReg1) = src.features.Split();
  std::tie(m_lbaLowReg0, m_lbaLowReg1) = src.lbaLow.Split();
  std::tie(m_lbaMidReg0, m_lbaMidReg1) = src.lbaMid.Split();
  std::tie(m_lbaHighReg0, m_lbaHighReg1) = src.lbaHigh.Split();
}

// ------------------------------------------------------ //
//  Queries
// ------------------------------------------------------ //

[[nodiscard]] constexpr 
std::uint8_t Frame::TypeValue() const noexcept{
  return m_type;
}

[[nodiscard]] constexpr bool Frame::IsC() const noexcept{
  return m_c_portMultiplier & 0x80;
}

[[nodiscard]] constexpr 
std::uint8_t Frame::SelectedPort() const noexcept{
  return m_c_portMultiplier & 0x0F;
}

[[nodiscard]] constexpr
std::uint8_t Frame::Command() const noexcept{
  return m_commandReg;
}

[[nodiscard]] constexpr
std::uint16_t Frame::Features() const noexcept{
  return (m_featuresReg1 << 8) | m_featuresReg0;
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

[[nodiscard]] constexpr
std::uint8_t Frame::Device() const noexcept{
  return m_deviceReg;
}

[[nodiscard]] constexpr
std::uint16_t Frame::SectorCount() const noexcept{
  return m_sectorCountReg;
}

[[nodiscard]] constexpr
std::uint8_t Frame::Control() const noexcept{
  return m_controlReg;
}

}

#endif
