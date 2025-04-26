#pragma once

#include <cstdint>

#include "alloc.hpp"
#include "linked.hpp"
#include "./../../list_descriptor.hpp"

namespace Mem::Heap::Test::List{
  inline void TestRun(){
    Alloc::TryDoubleDeallocate1();
  }
}
