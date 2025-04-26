#pragma once

#include "initial.hpp"
#include "alloc.hpp"
#include "dealloc.hpp"

namespace Mem::Page::Test::Freelist{

  namespace Initial{
    void Init();
  }

  inline void TestRun(){
    Initial::Init();
  }

}
