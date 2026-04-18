#pragma once

// Main interface to the page frame manager 

#include <cstdint>

#include "frame_array.hpp"

#include "concepts/allocator.hpp"
#include "concepts/kglobal.hpp"

namespace Chisaka{
  template<typename A>
    requires Concepts::PageAllocator<A> && Concepts::KGlobalObject<A>
  class Ram{
    public:
      static Ram& Get(){static Ram g_ram; return g_ram;}
      
      void Init(){A::Get().Init();}

      void* Allocate(){ return A::Get().AllocatePage();}
      void* Allocate(unsigned npages){ return A::Get().AllocatePages(npages);}
      void Deallocate(void* base){ return A::Get().DeallocPage(base);}

    private:
      FrameArray m_frames;
  };
}
