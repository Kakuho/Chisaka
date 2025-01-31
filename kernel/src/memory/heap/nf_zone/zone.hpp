#ifndef MEMORY_HEAP_NFZONE_ZONE_HPP
#define MEMORY_HEAP_NFZONE_ZONE_HPP

#include <cstdint>

namespace Mem::Heap::NFZone::T{

template <typename T>
class Zone{
  public:
    explicit Zone(std::size_t alloc);

  private:
    T* m_firstFree;
};


}

#endif
