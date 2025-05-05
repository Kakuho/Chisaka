#pragma once

#include "process.hpp"

namespace Proc{

Process* Current();
void Dump();

void ContextSwitch(Process& process);

}
