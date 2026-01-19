#pragma once

#include "kcontext.hpp"

namespace Kernel{
  inline void* palloc(){
    return Chisaka::KContext::gRam::Get().Allocate();
  }

  inline void* palloc(unsigned pages){
    return Chisaka::KContext::gRam::Get().Allocate(pages);
  }

  inline void pfree(void* pagebase){
    return Chisaka::KContext::gRam::Get().Deallocate(pagebase);
  }

} // namespace Kernel
