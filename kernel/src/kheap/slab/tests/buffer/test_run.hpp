#pragma once

#include <cstdint>

#include "list_management.hpp"
#include "alloc.hpp"
#include "dealloc.hpp"

namespace Mem::Heap::Test::Buffer{

  void TestRun(){
    Dealloc::Thrash1();
  }

}
//-------------------------------------------------------------
