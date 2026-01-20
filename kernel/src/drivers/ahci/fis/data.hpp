#pragma once

#include <cstdint>
#include <cassert>

#include "fis_types.hpp"

namespace Chisaka::Ahci{

class DataFis{
  static constexpr std::uint8_t TYPE_VALUE = 
    GetUnderlying(FisType::DMAActivate);

  public:
    constexpr DataFis() noexcept;
  
  public:
    // Dword 0
    const std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_rsv_pmport;
    std::uint16_t m_rsv0 = 0;
    // Dword 1 - N
    std::uint32_t payload[1];
};

//-----------------------------------------------------------------------------------------

constexpr DataFis::DataFis() noexcept
  : 
    m_rsv_pmport{0}
{

}

}
