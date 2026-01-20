#include "h2d_register.hpp"

namespace Chisaka::Ahci{

H2dRegisterFis H2dRegisterFis::CreateWrite(std::uint64_t sector_address){
  static constexpr std::uint8_t WRITE_DMA_EXT_CODE = 0x35;

  auto addresses = ExtractAddress(sector_address);
  H2dRegisterFis dmaWriteFis{};
  kassert(dmaWriteFis.m_type == 0x27);

  dmaWriteFis.m_sectorCountReg = 1;
  // LBA
  dmaWriteFis.m_lbaLowReg0    = std::get<0>(addresses);
  dmaWriteFis.m_lbaMidReg0    = std::get<1>(addresses);
  dmaWriteFis.m_lbaHighReg0   = std::get<2>(addresses);
  // Expanded
  dmaWriteFis.m_lbaLowReg1    = std::get<3>(addresses);
  dmaWriteFis.m_lbaMidReg1    = std::get<4>(addresses);
  dmaWriteFis.m_lbaHighReg1   = std::get<5>(addresses);
  // Rest of the features
  dmaWriteFis.m_deviceReg = 0x40;
  dmaWriteFis.m_c_portMultiplier = 0x80;
  dmaWriteFis.m_commandReg = WRITE_DMA_EXT_CODE;
  return dmaWriteFis;
}

H2dRegisterFis H2dRegisterFis::CreateRead(std::uint64_t sector_address){
  static constexpr std::uint8_t READ_DMA_EXT_CODE = 0x25;

  auto addresses = ExtractAddress(sector_address);
  H2dRegisterFis dmaReadFis{};
  kassert(dmaReadFis.m_type == 0x27);

  dmaReadFis.m_sectorCountReg = 1;
  // LBA
  dmaReadFis.m_lbaLowReg0    = std::get<0>(addresses);
  dmaReadFis.m_lbaMidReg0    = std::get<1>(addresses);
  dmaReadFis.m_lbaHighReg0   = std::get<2>(addresses);
  // Expanded
  dmaReadFis.m_lbaLowReg1    = std::get<3>(addresses);
  dmaReadFis.m_lbaMidReg1    = std::get<4>(addresses);
  dmaReadFis.m_lbaHighReg1   = std::get<5>(addresses);
  // Rest of the features
  dmaReadFis.m_deviceReg = 0x40;
  dmaReadFis.m_c_portMultiplier = 0x80;
  dmaReadFis.m_commandReg = READ_DMA_EXT_CODE;
  return dmaReadFis;
}

H2dRegisterFis H2dRegisterFis::CreateIdentifyDevice(){
  H2dRegisterFis identifyDeviceFis{};
  identifyDeviceFis.m_commandReg = 0xec;    // 0xec is IDENTIFY DEVICE
  kassert(identifyDeviceFis.m_type == 0x27);
  identifyDeviceFis.m_deviceReg = 0;
  identifyDeviceFis.m_c_portMultiplier = 0x80;
  return identifyDeviceFis;
}

H2dRegisterFis H2dRegisterFis::CreateReadMaxAddress(){
  static constexpr std::uint8_t READ_MAX_ADDRESS_CODE = 0xf8;
  H2dRegisterFis readMaxAddressFis{};
  readMaxAddressFis.m_commandReg = READ_MAX_ADDRESS_CODE;
  readMaxAddressFis.m_deviceReg = 0x40;
  return readMaxAddressFis;
}

H2dRegisterFis H2dRegisterFis::CreateInitDevH2dRegisterFis(){
  static constexpr std::uint8_t INII_DEV_CODE = 0x91;
  H2dRegisterFis initDevFis{};
}

}
