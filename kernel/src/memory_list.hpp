/*
#include <cstdint>
#include <limine.h>
#include <logger.hpp>

namespace{
  volatile limine_memmap_request memorymap_request{
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
    .response = nullptr
};

}

namespace memory{

enum class memblock_type {occupied, hole};

struct memblock{
  std::uint64_t base;
  std::uint64_t length;
  memblock_type type;
};

class memlist{
  public:
    memlist();
    memblock& operator[](std::size_t index);
};
*/
