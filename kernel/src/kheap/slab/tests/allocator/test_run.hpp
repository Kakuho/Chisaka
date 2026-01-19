#pragma once

#include <cstdint>

#include "initial.hpp"
#include "alloc.hpp"

namespace Mem::Heap::Test::Allocator{

  namespace Initial{
    void FixedAddr();
  }

  namespace Alloc{
    void AllocateDebug();
    void Allocate();
  }

  inline void TestRun(){
    Alloc::AllocateCLBFis();
  }
}
