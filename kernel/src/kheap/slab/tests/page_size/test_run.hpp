#pragma once

#include "list.hpp"
//#include "buffer_alloc.hpp"
#include "buffer_dealloc.hpp"

namespace Mem::Heap::Test::PageSize{
  inline void TestRun(){
    Buffer::MoveToFree_FullEntry();
  }
}
