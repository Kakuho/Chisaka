#include "frame_descriptor.hpp"

namespace Chisaka{

FrameDescriptor::FrameDescriptor(Chisaka::PhysAddr base, PageType type)
  : m_base{base},
    m_type{type},
    m_flags{0},
    m_procCount{0}
{

}

}
