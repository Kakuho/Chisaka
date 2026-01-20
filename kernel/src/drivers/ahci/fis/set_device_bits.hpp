#pragma once

//  Encapsulates Set Device Bits - Device to Host FISes
//
//  These FISes are used by the device to set the shadow block registers
//  which cannot otherwise be obtained, notably parts of the Error 
//  Register and the Status Register

#include <cstdint>
#include <cassert>

#include "fis_types.hpp"
#include "args.hpp"

namespace Chisaka::Ahci{

struct SetDeviceBitsInitialiser{
  Args::PortMultiplier portMultiplier;
  Args::Interrupt interrupt;
  Args::Notification notification;
  Args::Status status;
  Args::Error error;
};

class SetDeviceBitsFis{
  static constexpr std::uint8_t TYPE_VALUE = 
    GetUnderlying(FisType::SetDeviceBits);

  public:
    constexpr SetDeviceBitsFis() = default;
    constexpr SetDeviceBitsFis(SetDeviceBitsInitialiser&& src) noexcept;

    constexpr std::uint8_t Type() const noexcept { return m_type;}
    constexpr std::uint8_t StatusByte() const noexcept { return m_statusByte;}
    constexpr std::uint8_t Error() const noexcept { return m_errorReg;}

    // Status Querying
    constexpr bool BSY() const noexcept;
    constexpr bool DRDY() const noexcept;
    constexpr bool DRQ() const noexcept;
    constexpr bool ERR() const noexcept;

  private:
    constexpr void
    FormPack(Args::Notification, Args::Interrupt, Args::PortMultiplier) noexcept;

  public:
    // Public Open Encapsulation for now... will change it later on

    // Dword 0
    const std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_n_i_portMultiplier;
    std::uint8_t m_statusByte;
    std::uint8_t m_errorReg;
    // Dword 1
    std::uint32_t m_rsv = 0;
};

static_assert(sizeof(SetDeviceBitsFis) == 8);

constexpr SetDeviceBitsFis::SetDeviceBitsFis(SetDeviceBitsInitialiser&& src) noexcept
  : m_n_i_portMultiplier{0},
    m_statusByte{src.status.data},
    m_errorReg{src.error.data}
{
  FormPack(src.notification, src.interrupt, src.portMultiplier);
}

constexpr void SetDeviceBitsFis::FormPack(Args::Notification n, Args::Interrupt i, Args::PortMultiplier pm) 
noexcept{
  m_n_i_portMultiplier |= n ? 0x80 : 0x00;
  m_n_i_portMultiplier |= i ? 0x40 : 0x00;
  m_n_i_portMultiplier |= pm.data & 0xf;
}

}
