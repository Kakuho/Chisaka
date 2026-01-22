#include "doctest.h"

#include <iostream>
#include <memory>
#include <vector>

#include "mocks/page.hpp"
#include "kheap/slab/buffer.hpp"

using namespace Chisaka;


bool DoInitCheck(std::uint16_t bufferSize){
  Slab::Buffer buf{bufferSize};

  CHECK(buf.BufferSize() == bufferSize);
  CHECK(buf.FreeListHead() == nullptr);
  CHECK(buf.PartialListHead() == nullptr);
  CHECK(buf.FullListHead() == nullptr);
  return true;
}

TEST_CASE("Buffer of size > 8 initialises correctly") {
  std::vector<int> data{1, 4, 8, 16, 32, 64, 128, 256, 512};
  for(auto i: data){
    CHECK(DoInitCheck(i));
  }
}

bool DoSmallInitCheck(std::uint16_t bufferSize){
  Slab::Buffer buf{bufferSize};

  CHECK(buf.BufferSize() == 8);
  CHECK(buf.FreeListHead() == nullptr);
  CHECK(buf.PartialListHead() == nullptr);
  CHECK(buf.FullListHead() == nullptr);
  return true;
}

TEST_CASE("Buffer of size < 8 initialises correctly") {
  std::vector<int> data{1, 2, 4, 6, 8};
  for(auto i: data){
    CHECK(DoSmallInitCheck(i));
  }
}
