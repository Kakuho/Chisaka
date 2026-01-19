#pragma once

#include "kcontext.hpp"

namespace Kernel{
  inline void* palloc(){
    return Chisaka::KContext::Ram::Get().Allocate();
  }

  inline void* palloc(unsigned pages){
    return Chisaka::KContext::Ram::Get().Allocate(pages);
  }

  inline void pfree(void* pagebase){
    return Chisaka::KContext::Ram::Get().Deallocate(pagebase);
  }

} // namespace Kernel
