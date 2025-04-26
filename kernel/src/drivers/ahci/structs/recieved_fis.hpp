#ifndef DRIVERS_AHCI_RECIEVED_FIS_HPP
#define DRIVERS_AHCI_RECIEVED_FIS_HPP

// Each ahci port requires an recieved fis data structure


#include <cstdint>

#include "drivers/sata/fis/dma_setup.hpp"
#include "drivers/sata/fis/pio_setup.hpp"
#include "drivers/sata/fis/d2h_register.hpp"
#include "drivers/sata/fis/set_device_bits.hpp"

namespace Drivers::Ahci{

struct RecievedFis{
  Sata::Fis::DmaSetup::Frame m_dsfis;
  [[maybe_unused]] std::uint8_t padding0[4];

  Sata::Fis::PioSetup::Frame m_psfis;
  [[maybe_unused]] std::uint8_t padding1[12];

  Sata::Fis::D2HRegister::Frame m_rfis;
  [[maybe_unused]] std::uint8_t padding2[4];

  Sata::Fis::SetDeviceBits::Frame m_sdbfis;
  std::uint8_t m_ufis[64];

  std::uint8_t m_rsv[0x100 - 0xa0];
};

static_assert(sizeof(RecievedFis) == 0x100);

}

#endif
