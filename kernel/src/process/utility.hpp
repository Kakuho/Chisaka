#pragma once

#include "process.hpp"
#include "x86_64/paging/upper_page_table.hpp"

namespace Proc{

Process* Current();

void Dump();

void ContextSwitch(Process& process);

void LinkProcessTable(const Process& process, const X8664::UpperPageTable& pagetable);

}
