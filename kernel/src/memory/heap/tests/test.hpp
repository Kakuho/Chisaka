#pragma once

#include "list/test_run.hpp"
#include "buffer/test_run.hpp"
#include "allocator/test_run.hpp"

namespace Mem::Heap::Test{

  void Start(){
    Allocator::TestRun();
  }

}
