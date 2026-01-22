#include "doctest.h"  

#include <memory>

#include "mocks/page.hpp"
#include "kheap/slab/buffer.hpp"
#include "kheap/slab/list_descriptor.hpp"

using namespace Chisaka;

TEST_CASE("Buffer: Adding a free list descriptor should be to the free list"){
  auto page = std::make_unique<Mocks::Page>();

  Slab::ListDescriptor desc{page.get(), 64, 1};
  Slab::Buffer buf{64};
  buf.AddList(&desc);

  CHECK((buf.FreeListHead() == &desc));
  CHECK((buf.PartialListHead() == nullptr));
  CHECK((buf.FullListHead() == nullptr));
}

TEST_CASE("Buffer: List Descriptors gets added to the list"){
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

  Slab::Buffer buf{64};

  // d1 d3 shall be partial, d2 d4 shall be free
  desc1.Allocate();
  desc3.Allocate();

  buf.AddList(&desc1);
  CHECK((buf.FreeListHead() == nullptr));
  CHECK((buf.PartialListHead() == &desc1));

  buf.AddList(&desc2);
  CHECK((buf.FreeListHead() == &desc2));
  CHECK((buf.PartialListHead() == &desc1));

  buf.AddList(&desc3);
  CHECK((buf.FreeListHead() == &desc2));
  CHECK((buf.PartialListHead() == &desc1));
  CHECK((buf.PartialListHead()->NextList() == &desc3));

  buf.AddList(&desc4);
  CHECK((buf.FreeListHead() == &desc2));
  CHECK((buf.FreeListHead()->NextList() == &desc4));
  CHECK((buf.PartialListHead() == &desc1));
  CHECK((buf.PartialListHead()->NextList() == &desc3));
}

TEST_CASE("Buffer: List Descriptors gets added to the correct list"){
  auto page1 = std::make_unique<Mocks::Page>();
  auto page2 = std::make_unique<Mocks::Page>();
  auto page3 = std::make_unique<Mocks::Page>();
  auto page4 = std::make_unique<Mocks::Page>();
  auto page5 = std::make_unique<Mocks::Page>();
  auto page6 = std::make_unique<Mocks::Page>();

  Slab::ListDescriptor desc1{page1.get(), 64, 1};
  Slab::ListDescriptor desc2{page2.get(), 64, 1};
  Slab::ListDescriptor desc3{page3.get(), 64, 1};
  Slab::ListDescriptor desc4{page4.get(), 64, 1};
  Slab::ListDescriptor desc5{page5.get(), 64, 1};
  Slab::ListDescriptor desc6{page6.get(), 64, 1};

  Slab::Buffer buf{64};

  desc1.SetupLinkage();
  desc2.SetupLinkage();
  desc3.SetupLinkage();
  desc4.SetupLinkage();
  desc5.SetupLinkage();
  desc6.SetupLinkage();

  // d1 d3 shall be partial,

  desc1.Allocate();
  desc3.Allocate();

  // d2 d4 shall be free

  // d5, d6 are full list descriptors
  for(auto i = 0; i < desc5.TotalBuffers(); i++){
    [[maybe_unused]] auto* leaky = desc5.Allocate();
  }

  for(auto i = 0; i < desc6.TotalBuffers(); i++){
    [[maybe_unused]] auto* leaky = desc6.Allocate();
  }

  // now we check the conditions

  buf.AddList(&desc1);
  CHECK((buf.FreeListHead() == nullptr));
  CHECK((buf.PartialListHead() == &desc1));
  CHECK((buf.FullListHead() == nullptr));

  buf.AddList(&desc2);
  CHECK((buf.FreeListHead() == &desc2));
  CHECK((buf.PartialListHead() == &desc1));
  CHECK((buf.FullListHead() == nullptr));

  buf.AddList(&desc3);
  CHECK((buf.FreeListHead() == &desc2));
  CHECK((buf.PartialListHead() == &desc1));
  CHECK((buf.PartialListHead()->NextList() == &desc3));
  CHECK((buf.FullListHead() == nullptr));

  buf.AddList(&desc4);
  CHECK((buf.FreeListHead() == &desc2));
  CHECK((buf.FreeListHead()->NextList() == &desc4));
  CHECK((buf.PartialListHead() == &desc1));
  CHECK((buf.PartialListHead()->NextList() == &desc3));
  CHECK((buf.FullListHead() == nullptr));

  buf.AddList(&desc5);
  CHECK((buf.FreeListHead() == &desc2));
  CHECK((buf.FreeListHead()->NextList() == &desc4 &&
      buf.FreeListHead()->PrevList() == &desc4));

  CHECK((buf.PartialListHead() == &desc1));
  CHECK((buf.PartialListHead()->NextList() == &desc3 &&
      buf.PartialListHead()->PrevList() == &desc3));

  CHECK((buf.FullListHead() == &desc5));
  CHECK((buf.FullListHead()->NextList() == &desc5 &&
      buf.FullListHead()->PrevList() == &desc5));

  buf.AddList(&desc6);
  CHECK((buf.FreeListHead() == &desc2));
  CHECK((buf.FreeListHead()->NextList() == &desc4));

  CHECK((buf.PartialListHead() == &desc1));
  CHECK((buf.PartialListHead()->NextList() == &desc3));

  CHECK((buf.FullListHead() == &desc5));
  CHECK((buf.FullListHead()->NextList() == &desc6));
  CHECK((buf.FullListHead()->PrevList() == &desc6));
}
