#include "doctest.h"  

#include <memory>

#include "mocks/page.hpp"
#include "kheap/slab/buffer.hpp"
#include "kheap/slab/list_descriptor.hpp"

using namespace Chisaka;

TEST_CASE("Buffer: Allocation to a Buffer which has a free list descriptor should move it to the partial list"){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{page.get(), 10, 1};
  desc.SetupLinkage();

  Slab::Buffer buf{10};
  buf.AddList(&desc);

  CHECK(buf.FreeListHead() == &desc);
  CHECK(buf.PartialListHead() == nullptr);
  CHECK(buf.FullListHead() == nullptr);

  void* nextfree = desc.NextFreeBuffer();
  void* allocd = buf.Allocate();

  CHECK(buf.FreeListHead() == nullptr);
  CHECK(buf.PartialListHead() == &desc);
  CHECK(buf.FullListHead() == nullptr);

  kassert(allocd == nextfree);

  void* nextfree2 = desc.NextFreeBuffer();
  void* allocd2 = buf.Allocate();

  kassert(allocd2 == nextfree2);
  kassert(allocd2 != allocd);
}


TEST_CASE("Buffer: Allocation to a Buffer which has a free list descriptor and a partial list descriptor, should come from the partial list first"){

  auto page1 = std::make_unique<Mocks::Page>();
  auto page2 = std::make_unique<Mocks::Page>();
  auto page3 = std::make_unique<Mocks::Page>();
  auto page4 = std::make_unique<Mocks::Page>();

  Slab::ListDescriptor desc1{page1.get(), 64, 1};
  Slab::ListDescriptor desc2{page2.get(), 64, 1};
  Slab::ListDescriptor desc3{page3.get(), 64, 1};
  Slab::ListDescriptor desc4{page4.get(), 64, 1};

  desc1.SetupLinkage();
  desc2.SetupLinkage();
  desc3.SetupLinkage();
  desc4.SetupLinkage();

  // d1 d3 shall be partial, d2 d4 shall be free
  desc1.Allocate();
  desc3.Allocate();

  // setting up the buffer
  Slab::Buffer buf{64};

  buf.AddList(&desc1);
  buf.AddList(&desc2);
  buf.AddList(&desc3);
  buf.AddList(&desc4);

  // ensuring the correct order of lists
  REQUIRE(buf.FreeListHead() == &desc2);
  REQUIRE(buf.FreeListHead()->NextList() == &desc4);
  REQUIRE(buf.FreeListHead()->PrevList() == &desc4);
  REQUIRE(buf.PartialListHead() == &desc1);
  REQUIRE(buf.PartialListHead()->NextList() == &desc3);
  REQUIRE(buf.PartialListHead()->PrevList() == &desc3);
  REQUIRE(buf.FullListHead() == nullptr);

  // the next allocated should be from desc1

  void* nextfree = desc1.NextFreeBuffer();
  void* allocd = buf.Allocate();

  CHECK(allocd == nextfree);

  // Ensure correct list linkages
  REQUIRE(buf.FreeListHead() == &desc2);
  REQUIRE(buf.FreeListHead()->NextList() == &desc4);
  REQUIRE(buf.FreeListHead()->PrevList() == &desc4);
  REQUIRE(buf.PartialListHead() == &desc1);
  REQUIRE(buf.PartialListHead()->NextList() == &desc3);
  REQUIRE(buf.PartialListHead()->PrevList() == &desc3);
  REQUIRE(buf.FullListHead() == nullptr);
}

TEST_CASE("Buffer: Exhaustion test 1 - 1xfree"){

  auto page1 = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc1{page1.get(), 64, 1};
  desc1.SetupLinkage();

  Slab::Buffer buf{64};
  buf.AddList(&desc1);

  REQUIRE(buf.FreeListHead() == &desc1);
  REQUIRE(buf.FreeListHead()->NextList() == &desc1);
  REQUIRE(buf.FreeListHead()->PrevList() == &desc1);
  REQUIRE(buf.PartialListHead() == nullptr);
  REQUIRE(buf.FullListHead() == nullptr);

  for(auto i = 0; i < desc1.TotalBuffers(); i++){
    // Allocate from the buffer - overshoots a bit
    buf.Allocate();
  }

  CHECK(buf.FreeListHead() == nullptr);
  CHECK(buf.PartialListHead() == nullptr);
  CHECK(buf.FullListHead() == &desc1);
  CHECK(buf.FullListHead()->NextList() == &desc1);
  CHECK(buf.FullListHead()->PrevList() == &desc1);
}


TEST_CASE("Buffer: Exhaustion test 2 - initially 2 free lists, exhaust 1, should have 1x free 1x full"){
  auto page1 = std::make_unique<Mocks::Page>();
  auto page2 = std::make_unique<Mocks::Page>();

  Slab::ListDescriptor desc1{reinterpret_cast<void*>(page1.get()), 64, 1};
  Slab::ListDescriptor desc2{reinterpret_cast<void*>(page2.get()), 64, 1};

  desc1.SetupLinkage();
  desc2.SetupLinkage();

  Slab::Buffer buf{64};

  buf.AddList(&desc1);
  buf.AddList(&desc2);

  // some precondiitons on the order of the lists
  REQUIRE(buf.FreeListHead() == &desc1);
  REQUIRE(buf.FreeListHead()->NextList() == &desc2);
  REQUIRE(buf.FreeListHead()->PrevList() == &desc2);
  REQUIRE(buf.PartialListHead() == nullptr);
  REQUIRE(buf.FullListHead() == nullptr);

  // exhausting the list descriptor from the buffer
  for(auto i = 0; i < desc1.TotalBuffers(); i++){
    buf.Allocate();
  }

  // checking that the conclusion is satisifed
  CHECK(buf.FullListHead() == &desc1);
  CHECK(buf.FullListHead()->NextList() == &desc1);
  CHECK(buf.FullListHead()->PrevList() == &desc1);

  CHECK(buf.FreeListHead() == &desc2);
  CHECK(buf.FreeListHead()->NextList() == &desc2);
  CHECK(buf.FreeListHead()->PrevList() == &desc2);

  // extra last check to see if the next buffer comes from the remaining free list
  void* nextfree = desc2.NextFreeBuffer();
  void* allocd = buf.Allocate();

  CHECK(allocd == nextfree);
}

TEST_CASE("Buffer: Exhaustion test 3 - initially 2 free lists, 2x partial list, 1x full list. Exhaust the partial list, should have 2x free 1x partial, 2x full"){
  auto page1 = std::make_unique<Mocks::Page>();
  auto page2 = std::make_unique<Mocks::Page>();
  auto page3 = std::make_unique<Mocks::Page>();
  auto page4 = std::make_unique<Mocks::Page>();
  auto page5 = std::make_unique<Mocks::Page>();

  Slab::ListDescriptor desc1{page1.get(), 64, 1};
  Slab::ListDescriptor desc2{page2.get(), 64, 1};
  Slab::ListDescriptor desc3{page3.get(), 64, 1};
  Slab::ListDescriptor desc4{page4.get(), 64, 1};
  Slab::ListDescriptor desc5{page5.get(), 64, 1};

  Slab::Buffer buf{64};

  desc1.SetupLinkage();
  desc2.SetupLinkage();
  desc3.SetupLinkage();
  desc4.SetupLinkage();
  desc5.SetupLinkage();

  
  // establishing the initial state of list descriptors

  // desc1, desc2 are partial, 
  // desc3 is full, 
  // desc4, desc5 are free.

  desc1.Allocate();
  desc2.Allocate();

  for(auto i = 0; i < desc3.TotalBuffers(); i++){
    desc3.Allocate();
  }

  buf.AddList(&desc1);
  buf.AddList(&desc2);
  buf.AddList(&desc3);
  buf.AddList(&desc4);
  buf.AddList(&desc5);

  REQUIRE(buf.FreeListHead() == &desc4);
  REQUIRE(buf.FreeListHead()->NextList() == &desc5);
  REQUIRE(buf.FreeListHead()->PrevList() == &desc5);

  REQUIRE(buf.PartialListHead() == &desc1);
  REQUIRE(buf.PartialListHead()->NextList() == &desc2);
  REQUIRE(buf.PartialListHead()->PrevList() == &desc2);

  REQUIRE(buf.FullListHead() == &desc3);

  //-------------------------------------------------------------

  // now we perfom the test logic proper

  for(auto i = 0; i < desc1.TotalBuffers() - 1; i++){
    buf.Allocate();
  }

  // buffer should allocate from the partial list first, so 
  // free list is not touched
  CHECK(buf.FreeListHead() == &desc4);
  CHECK(buf.FreeListHead()->NextList() == &desc5);
  CHECK(buf.FreeListHead()->PrevList() == &desc5);

  CHECK(buf.PartialListHead() == &desc2);
  CHECK(buf.PartialListHead()->NextList() == &desc2);
  CHECK(buf.PartialListHead()->PrevList() == &desc2);

  CHECK(buf.FullListHead() == &desc3);
  CHECK(buf.FullListHead()->NextList() == &desc1);
  CHECK(buf.FullListHead()->PrevList() == &desc1);

  // just some cute extra test

  void* nextfree = desc2.NextFreeBuffer();
  void* allocd = buf.Allocate();

  CHECK(allocd == nextfree);
}
