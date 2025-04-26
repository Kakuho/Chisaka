#ifndef DRIVERS_AHCI_COMMAND_LIST_HPP
#define DRIVERS_AHCI_COMMAND_LIST_HPP

// each AHCI port requires a Command List

#include <cstdint>
#include <cassert>

#include "command_header.hpp"
#include "primrose/static_array.hpp"

#include "lib/string.h"

namespace Drivers::Ahci{

class CommandList{
  public:
    CommandList() = default;

  public:
    Prim::StaticArray<CommandHeader, 32> m_headers;
};

static_assert(sizeof(CommandList) == 0x400);

//-------------------------------------------------------------

}

#endif
