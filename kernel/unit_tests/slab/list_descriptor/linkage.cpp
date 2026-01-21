#include "doctest.h"

#include <memory>

#include "mocks/page.hpp"
#include "kheap/slab/list_descriptor.hpp"

using namespace Chisaka;

TEST_CASE("List Descriptors of size one will link to itself ") {
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{reinterpret_cast<void*>(&page), 8, 1};
  kassert(desc.PrevList() == &desc);
  kassert(desc.NextList() == &desc);
}
