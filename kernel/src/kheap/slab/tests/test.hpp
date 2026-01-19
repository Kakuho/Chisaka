#pragma once

#include "list/test_run.hpp"
#include "buffer/test_run.hpp"
#include "allocator/test_run.hpp"
#include "page_size/test_run.hpp"

namespace Mem::Heap::Test{

  void Start(){
    PageSize::TestRun();
  }

}
