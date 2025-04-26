#ifndef DRIVERS_SATA_COMMANDS_HPP
#define DRIVERS_SATA_COMMANDS_HPP

#include <cstdint>

namespace Drivers::Sata{

enum class Commands: std::uint8_t{
  IdentifyDevice = 0xec
};

}

#endif
