#include <cstdint>

//  how do you ensure that the internal data structures remain conssistent after things like double frees?

namespace Chisaka::Tests{
  enum class DeallocationFailure{
    NotAllocated,                       // the memory address has not been allocated, freeing an error
    IsNull,
  };

  enum class AllocationFailure{
    AlreadyAllocated,                   // the memory address has already been allocated
    NotAligned,                         // the memory address is not aligned to the data size
    IsNull,
  };
}
