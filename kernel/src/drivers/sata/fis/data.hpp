#ifndef DRIVERS_SATA_FIS_DATA_HPP
#define DRIVERS_SATA_FIS_DATA_HPP

#include <cstdint>
#include <cassert>

#include "fis_types.hpp"

namespace Drivers::Sata::Fis::Data{

class Frame{
  static constexpr std::uint8_t TYPE_VALUE = 
    GetUnderlying(FisType::DMAActivate);

  public:
    
    constexpr Frame() noexcept;
  
  public:
    // Dword 0
    const std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_rsv_pmport;
    std::uint16_t m_rsv0 = 0;
    // Dword 1 - N
    std::uint32_t payload[1];
};

constexpr Frame::Frame() noexcept
  : 
    m_rsv_pmport{0}
{

}

} // namespace Drivers::Sata::Fis::Data

#endif
