#ifndef DRIVERS_SATA_FIS_SET_DEVICE_BITS_HPP
#define DRIVERS_SATA_FIS_SET_DEVICE_BITS_HPP

//  Encapsulates Set Device Bits - Device to Host FISes
//
//  These FISes are used by the device to set the shadow block registers
//  which cannot otherwise be obtained, notably parts of the Error 
//  Register and the Status Register

#include <cmath>
#include <cstdint>
#include <cassert>

#include "fis_types.hpp"
#include "args.hpp"

namespace Drivers::Sata::Fis::SetDeviceBits{

struct Initialiser{
  Args::PortMultiplier portMultiplier;
  Args::Interrupt interrupt;
  Args::Notification notification;
  Args::Status status;
  Args::Error error;
};

class Frame{
  static constexpr std::uint8_t TYPE_VALUE = 
    static_cast<std::uint8_t>(FisType::SetDeviceBits);

  public:
    constexpr Frame() = default;
    constexpr Frame(Initialiser&& src) noexcept;

    [[nodiscard]] constexpr std::uint8_t Type() const noexcept{ return m_type;}
    [[nodiscard]] constexpr std::uint8_t StatusByte() const noexcept;
    [[nodiscard]] constexpr std::uint8_t Error() const noexcept{ return m_errorReg;}

    // Status Querying
    [[nodiscard]] constexpr bool BSY() const noexcept;
    [[nodiscard]] constexpr bool DRDY() const noexcept;
    [[nodiscard]] constexpr bool DRQ() const noexcept;
    [[nodiscard]] constexpr bool ERR() const noexcept;

  private:
    constexpr void
    FormPack(Args::Notification, Args::Interrupt, Args::PortMultiplier) noexcept;

  private:
    // Dword 0
    std::uint8_t m_type = TYPE_VALUE;
    std::uint8_t m_n_i_portMultiplier;
    std::uint8_t m_statusByte;
    std::uint8_t m_errorReg;
};

static_assert(sizeof(Frame) == 4);

constexpr Frame::Frame(Initialiser&& src) noexcept
  : m_n_i_portMultiplier{0},
    m_statusByte{src.status.data},
    m_errorReg{src.error.data}
{
  FormPack(src.notification, src.interrupt, src.portMultiplier);
}

constexpr void
Frame::FormPack(Args::Notification n, Args::Interrupt i, Args::PortMultiplier pm) 
noexcept{
  m_n_i_portMultiplier |= n ? 0x80 : 0x00;
  m_n_i_portMultiplier |= i ? 0x40 : 0x00;
  m_n_i_portMultiplier |= pm.data & 0xf;
}

} // namespace Drivers::Sata::Fis::SetDeviceBits
  
#endif
