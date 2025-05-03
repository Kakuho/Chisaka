#include "frame_descriptor.hpp"
#include "memory/memory_map/memory_map_entry.hpp"

namespace Mem::Page{

FrameDescriptor::FrameDescriptor(Mem::physaddr_t base, PageType type)
  : m_base{base},
    m_type{type},
    m_flags{0},
    m_procCount{0}
{

}

}
