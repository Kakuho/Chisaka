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

      void* Allocate(){ A::Get().AllocatePage();}
      void* Allocate(unsigned npages){ A::Get().AllocatePages(npages);}
      void Deallocate(void* base){ A::Get().DeallocPage(base);}

    private:
      FrameArray m_frames;
  };
}
