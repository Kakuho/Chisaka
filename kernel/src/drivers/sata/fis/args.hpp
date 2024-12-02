#ifndef DRIVERS_SATA_FIS_ARGS_HPP
#define DRIVERS_SATA_FIS_ARGS_HPP

// Type safe classes for SATA FIS constructor arguments

#include <cstdint>
#include <tuple>

#include "lib/kassert.hpp"

namespace Drivers::Sata::Args{

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
}

#endif
