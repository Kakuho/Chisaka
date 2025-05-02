#pragma once

//  Type safe classes for SATA FIS constructor arguments
//
//  Arguments:
//    * C
//    * Port Multiplier
//    * Command
//    * Features
//    * LbaLow
//    * LbaMid
//    * LbaHigh
//    * Device
//    * SectorCount
//    * Control
//    * Interrupt
//    * Status
//    * Error

#include <cstdint>
#include <tuple>

#include "kassert.hpp"

namespace Drivers::Sata::Args{

// Common Args

// Register Host - Device

struct C{
  constexpr explicit C() noexcept = default;
  constexpr explicit C(bool src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator bool() const noexcept{
    return data;
  }

  bool data; 
};

struct PortMultiplier{
  constexpr explicit PortMultiplier() noexcept = default;
  constexpr explicit PortMultiplier(std::uint8_t src) noexcept: data{src}{
    kassert(data < 0xF);
  }

  [[nodiscard]] constexpr explicit operator std::uint8_t() const noexcept{
    return data;
  }

  std::uint8_t data; 
};

struct Command{
  constexpr explicit Command() noexcept = default;
  constexpr explicit Command(std::uint8_t src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator std::uint8_t() const noexcept{
    return data;
  }

  std::uint8_t data; 
};

struct Features{
  constexpr explicit Features() noexcept = default;
  constexpr explicit Features(std::uint16_t src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator std::uint16_t() const noexcept{
    return data;
  }

  [[nodiscard]] constexpr std::tuple<std::uint8_t, std::uint8_t> Split() const noexcept{
    return std::tuple{data & 0xFF, (data >> 8) & 0xFF};
  }

  std::uint16_t data; 
};

struct LbaLow{
  constexpr explicit LbaLow() noexcept = default;
  constexpr explicit LbaLow(std::uint16_t src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator std::uint16_t() const noexcept{
    return data;
  }

  [[nodiscard]] constexpr std::tuple<std::uint8_t, std::uint8_t> Split() const noexcept{
    return std::tuple{data & 0xFF, (data >> 8) & 0xFF};
  }

  std::uint16_t data; 
};

struct LbaMid{
  constexpr explicit LbaMid() noexcept = default;
  constexpr explicit LbaMid(std::uint16_t src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator std::uint16_t() const noexcept{
    return data;
  }

  [[nodiscard]] constexpr std::tuple<std::uint8_t, std::uint8_t> Split() const noexcept{
    return std::tuple{data & 0xFF, (data >> 8) & 0xFF};
  }

  std::uint16_t data; 
};

struct LbaHigh{
  constexpr explicit LbaHigh() noexcept = default;
  constexpr explicit LbaHigh(std::uint16_t src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator std::uint16_t() const noexcept{
    return data;
  }

  [[nodiscard]] constexpr std::tuple<std::uint8_t, std::uint8_t> Split() const noexcept{
    return std::tuple{data & 0xFF, (data >> 8) & 0xFF};
  }

  std::uint16_t data; 
};

struct Device{
  constexpr explicit Device() noexcept = default;
  constexpr explicit Device(std::uint8_t src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator std::uint8_t() const noexcept{
    return data;
  }

  std::uint8_t data; 
};

struct SectorCount{
  constexpr explicit SectorCount() noexcept = default;
  constexpr explicit SectorCount(std::uint16_t src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator std::uint16_t() const noexcept{
    return data;
  }

  std::uint16_t data; 
};

struct Control{
  constexpr explicit Control() noexcept = default;
  constexpr explicit Control(std::uint8_t src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator std::uint8_t() const noexcept{
    return data;
  }
  
  std::uint8_t data; 
};

// Register Device - Host

struct Interrupt{
  constexpr explicit Interrupt() noexcept = default;
  constexpr explicit Interrupt(bool src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator bool() const noexcept{
    return data;
  }

  bool data; 
};

struct Notification{
  constexpr explicit Notification() noexcept = default;
  constexpr explicit Notification(bool src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator bool() const noexcept{
    return data;
  }

  bool data; 
};

struct Status{
  constexpr explicit Status() noexcept = default;
  constexpr explicit Status(std::uint8_t src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator std::uint8_t() const noexcept{
    return data;
  }

  std::uint8_t data; 
};

struct Error{
  constexpr explicit Error() noexcept = default;
  constexpr explicit Error(std::uint8_t src) noexcept: data{src}{}

  [[nodiscard]] constexpr explicit operator std::uint8_t() const noexcept{
    return data;
  }

  std::uint8_t data; 
};

} // namespace Drivers::Sata::Args
