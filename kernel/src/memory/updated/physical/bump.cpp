#include "bump.hpp"

namespace Mem::Phys{

Bump::Bump(AddrType start, AddrType end) noexcept: 
  m_start{NearestAlign<AddrType, 0x1000>(start)}, 
  m_end{NearestAlign<AddrType, 0x1000>(end)}
{
  kassert(m_start <= m_end);
}

// ------------------------------------------------------ //
// PMM Interface
// ------------------------------------------------------ //

Bump::AddrType Bump::AllocatePage() noexcept{
  if(m_start <= m_end){
    AddrType allocated = m_start;
    m_start += 0x1000;
    return allocated;
  }
  else{
    return m_end;
  }
}

} // namespace Mem::Phys
