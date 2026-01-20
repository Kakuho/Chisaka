#pragma once

//  Encapsulates DMA Setup - Bidirectional FISes
//
//  Used by both host and device in order to initialise their dma controllers.
//  It serves as the first step to perform DMA.
//
//  In the situation where a Host needs to send multiple Data FISes, 
//  the Host must wait for successful reception of a DMA Activate FIS
//  before they can carry on with sending the next Data FIS.

#include <cstdint>
#include <cassert>

#include "fis_types.hpp"

namespace Chisaka::Ahci{

class DmaSetupFis{
  static constexpr std::uint8_t TYPE_VALUE = GetUnderlying(FisType::DMASetup);

  public:
    constexpr DmaSetupFis() noexcept;

    constexpr std::uint8_t Type() const noexcept{ return m_type;}
    constexpr std::uint8_t PortMultiplier() const noexcept{ return m_a_i_d_portMultiplier & 0xF;}

  public:
    // For now a public interface, but later would change to private...
    // Dword 0
    std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_a_i_d_portMultiplier;
    [[maybe_unused]] std::uint16_t m_rsv0 = 0;
    // Dword 1
    std::uint32_t m_dmaBufferLow;
    // Dword 2
    std::uint32_t m_dmaBufferHigh;
    // Dword 3
    [[maybe_unused]] std::uint32_t m_rsv1 = 0;
    // Dword 4
    std::uint32_t m_dmaBufferOffset;
    // Dword 5
    std::uint32_t m_dmaTransferCount;
    // Dword 6
    [[maybe_unused]] std::uint32_t m_rsv2 = 0;
};

static_assert(sizeof(DmaSetupFis) == 28);

//-------------------------------------------------------------

constexpr DmaSetupFis::DmaSetupFis() noexcept
  :
    // Dword 0
    m_a_i_d_portMultiplier{0},
    // Dword 1
    m_dmaBufferLow{0},
    // Dword 2
    m_dmaBufferHigh{0},
    // Dword 3 ... 
    // Dword 4 
    m_dmaBufferOffset{0},
    // Dword 5
    m_dmaTransferCount{0}
{

}

}
