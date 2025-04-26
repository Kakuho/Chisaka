#pragma once

#include "freelist/test_run.hpp"

namespace Mem::Page::Test{

  inline void Start(){
    Freelist::TestRun();
  }

}
