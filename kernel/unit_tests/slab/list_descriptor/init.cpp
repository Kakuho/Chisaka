#include "doctest.h"

#include <memory>

#include "mocks/page.hpp"
#include "kheap/slab/list_descriptor.hpp"

using namespace Chisaka;

static bool PageEquals(Mocks::Page& page, std::uint8_t ch){
  for(std::size_t i = 0; i < 0x1000; i++){
    if(page.bytes[i] != ch){
      return false;
    }
  }
  return true;
}

TEST_CASE("List Descriptors will correctly poison itself") {
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{reinterpret_cast<void*>(page.get()), 8, 1};
  desc.PoisonFreeArea(0xBE);
  PageEquals(*page, 0xBE);
}
