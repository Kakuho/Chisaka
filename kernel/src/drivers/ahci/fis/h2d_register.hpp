#pragma once

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

namespace Chisaka::Ahci{

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

class H2dRegisterFis{
  static constexpr std::uint8_t TYPE_VALUE = 
    GetUnderlying(FisType::RegisterHostToDevice);

  public:
    constexpr H2dRegisterFis(Initialiser&& src) noexcept;
    constexpr H2dRegisterFis();

    // static functions to create command fises
    static H2dRegisterFis CreateWrite(std::uint64_t sector_address);
    static H2dRegisterFis CreateRead(std::uint64_t sector_address);
    static H2dRegisterFis CreateIdentifyDevice();
    static H2dRegisterFis CreateReadMaxAddress();
    static H2dRegisterFis CreateInitDevH2dRegisterFis();

    constexpr std::uint8_t TypeValue() const noexcept   { return m_type;}
    constexpr bool IsC() const noexcept                 { return m_c_portMultiplier & 0x80;}
    constexpr std::uint8_t SelectedPort() const noexcept{ return m_c_portMultiplier & 0x0F;}
    constexpr std::uint8_t Command() const noexcept     { return m_commandReg;}
    constexpr std::uint16_t Features() const noexcept   { return (m_featuresReg1 << 8) | m_featuresReg0;}
    constexpr std::uint16_t LbaLow() const noexcept     { return (m_lbaLowReg1 << 8) | m_lbaLowReg0;}
    constexpr std::uint16_t LbaMid() const noexcept     { return (m_lbaMidReg1 << 8) | m_lbaMidReg0;}
    constexpr std::uint16_t LbaHigh() const noexcept    { return (m_lbaHighReg1 << 8) | m_lbaHighReg0;}
    constexpr std::uint8_t Device() const noexcept      { return m_deviceReg;}
    constexpr std::uint16_t SectorCount() const noexcept{ return m_sectorCountReg;}
    constexpr std::uint8_t Control() const noexcept     { return m_controlReg;}

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

static_assert(sizeof(H2dRegisterFis) == 20);

// ------------------------------------------------------ //
//  Impl
// ------------------------------------------------------ //

constexpr H2dRegisterFis::H2dRegisterFis()
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
H2dRegisterFis::H2dRegisterFis(Initialiser&& src)
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

}
