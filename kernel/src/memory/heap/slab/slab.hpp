#include <cstdint>

namespace Mem::Heap::Slab{

struct SlabDescriptor{
  // information regarding a slab
  // A slab requires:
  //    A free list of object part of its page
  //    A doubly linked list to other slabs part of its cache
  //    A Colouring Area
  void* nextObject;
  void* pageBase;
};

enum class CacheType: std::uint8_t{
  CacheDescriptor,
  SlabDescriptor,
  AddressSpaceDescriptor
};

struct CacheDescriptor{
  CacheType type;
  CacheDescriptor* nextCache;
  void Grow(); // used to increase # of slabs for this cache
};

class SlabAllocator{ 
  public:

    void* ConstructCache(CacheType type);

    template<typename T>
    T* Allocate()
    {
      // General allocation routine
    }

    void Free(CacheDescriptor* pcache, void* pobject);

  private:
    CacheDescriptor cacheOfCache;
    CacheDescriptor slabCache;
    CacheDescriptor addressCache;
    CacheDescriptor generalCache;
};

// Template Specialised Allocation for Kernel Object Types

template<>
inline CacheDescriptor* SlabAllocator::Allocate<CacheDescriptor>()
{
  // do something to the cacheCache here
  return nullptr;
}

void DoSomething(){
  SlabAllocator slaballoc;
  CacheDescriptor* pcache = slaballoc.Allocate<CacheDescriptor>();

}

}
