#pragma once

// Each ahci port requires an recieved fis data structure

#include <cstdint>

#include "../fis/dma_setup.hpp"
#include "../fis/pio_setup.hpp"
#include "../fis/d2h_register.hpp"
#include "../fis/set_device_bits.hpp"

namespace Chisaka::Ahci{

struct RecievedFis{
  public:
    volatile D2HRegisterFis& Rfis(){
      return m_rfis;
    }

  public:
    DmaSetupFis m_dsfis;
    [[maybe_unused]] std::uint8_t padding0[4];

    PioSetupFis m_psfis;
    [[maybe_unused]] std::uint8_t padding1[12];

    D2HRegisterFis m_rfis;
    [[maybe_unused]] std::uint8_t padding2[4];

    SetDeviceBitsFis m_sdbfis;
    std::uint8_t m_ufis[64];

    std::uint8_t m_rsv[0x100 - 0xa0];
};

static_assert(sizeof(RecievedFis) == 0x100);

}
