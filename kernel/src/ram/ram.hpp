#pragma once

// Main interface to the page frame manager 

#include <cstdint>

#include "frame_array.hpp"

namespace Chisaka{
  template<typename A>
  class Ram{
    public:
      static Ram& Get(){static Ram g_ram; return g_ram;}
    private:
      FrameArray m_frames;
  };
}
