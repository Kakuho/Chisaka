#include "doctest.h"

#include <memory>
#include <vector>

#include "mocks/page.hpp"
#include "kheap/slab/list_descriptor.hpp"

using namespace Chisaka;

TEST_CASE("List Descriptors will set up their linked list of buffer correctly") {
  // not sure how to do this tbh...
  CHECK(false);
}

TEST_CASE("List Descriptors will allocate memory contigously") {
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{reinterpret_cast<void*>(page.get()), 8, 1};
  desc.SetupLinkage();

  std::uint64_t* obj1 = static_cast<std::uint64_t*>(desc.Allocate());
  *obj1 = 100;

  std::uint64_t* obj2 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
  *obj2 = 0;

  std::uint64_t* obj3 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
  *obj3 = 0x02323;

  std::uint64_t* obj4 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
  *obj4 = 0xDEADBEEFDEADBEEF;

  // handle to the page bytes
  std::uint64_t* pagemem = reinterpret_cast<std::uint64_t*>(&page->bytes[0]); 

  CHECK(pagemem[0] == 100);
  CHECK(pagemem[1] == 0);
  CHECK(pagemem[2] == 0x02323);
  CHECK(pagemem[3] == 0xDEADBEEFDEADBEEF);
}

static bool DoExhaustMemoryTest(std::uint16_t buffer_size){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{reinterpret_cast<void*>(page.get()), 8, 1};
  desc.SetupLinkage();

  // exhausting the list 
  std::uint64_t* leaky = nullptr;
  for(auto i = 0; i < desc.TotalBuffers() ; i++){
    leaky = reinterpret_cast<std::uint64_t*>(desc.Allocate());
  }

  std::uint64_t* isNull = reinterpret_cast<std::uint64_t*>(desc.Allocate());

  REQUIRE(isNull == nullptr);
  REQUIRE(desc.NextFreeBuffer() == nullptr);
  REQUIRE(desc.BuffersUsed() == desc.TotalBuffers());
  REQUIRE(desc.Full());
  REQUIRE(!desc.HasSpace());
  return true;
}

TEST_CASE("List Descriptors should reflect exhausted memory") {
  std::vector<std::uint16_t> buffer_sizes {8, 16, 24, 32, 64};
  for(auto& i: buffer_sizes){
    CHECK(DoExhaustMemoryTest(i));
  }
}
