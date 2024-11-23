#include <cstdint>

namespace Mem::Heap::Zone{

template<typename T>
struct ZoneFreeListEntry{
  ZoneFreeListEntry<T>* next;
};

class ZoneDescriptorBase{
  // this just acts as a common interface for all zone descriptors
};

template<typename T>
struct ZoneDescriptor: ZoneDescriptorBase{
  ZoneFreeListEntry<T>* nextFree;
  std::size_t objBytes;
};

class Zone{

  private:
    ZoneDescriptorBase* zone_head;
};

}
