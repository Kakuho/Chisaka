#include "doctest.h"

#include <iostream>
#include <memory>

#include "mocks/page.hpp"
#include "kheap/slab/buffer.hpp"

using namespace Chisaka;

TEST_CASE("Buffer of size 10 initialises correctly") {
  Slab::Buffer buf{10};
  CHECK(buf.BufferSize() == 10);
  CHECK(buf.FreeListHead() == nullptr);
  CHECK(buf.PartialListHead() == nullptr);
  CHECK(buf.FullListHead() == nullptr);
}
