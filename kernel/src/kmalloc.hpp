#pragma once

#include <cstdint>

#include "kcontext.hpp"

namespace Kernel{
  using namespace Chisaka;
  inline void* kmalloc(std::size_t bytes){
    return KContext::KHeap::Get().Allocate(bytes);
  }
}
