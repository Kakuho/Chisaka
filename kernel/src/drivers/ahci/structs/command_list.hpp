#pragma once

// each AHCI port requires a Command List

#include <cstdint>
#include <cassert>

#include "command_header.hpp"
#include "aii/array.hpp"

#include "lib/string.h"

namespace Drivers::Ahci{

class CommandList{
  public:
    CommandList() = default;

  public:
    Aii::Array<CommandHeader, 32> m_headers;
};

static_assert(sizeof(CommandList) == 0x400);

//-------------------------------------------------------------

}
