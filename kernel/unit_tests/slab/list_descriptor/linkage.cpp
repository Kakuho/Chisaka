#include "doctest.h"

#include <memory>

#include "mocks/page.hpp"
#include "kheap/slab/list_descriptor.hpp"

using namespace Chisaka;

TEST_CASE("List Descriptors of size one will link to itself") {
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{reinterpret_cast<void*>(page.get()), 8, 1};
  CHECK(desc.PrevList() == &desc);
  CHECK(desc.NextList() == &desc);
}

TEST_CASE("List Descriptors cannot add themselves"){
  auto page1 = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc1{reinterpret_cast<void*>(page1.get()), 8, 1};

  desc1.AddList(&desc1);

  CHECK(desc1.PrevList() == &desc1);
  CHECK(desc1.PrevList() == &desc1);
}

TEST_CASE("List Descriptors will maintain the correct linkages when size is 2") {
  auto page1 = std::make_unique<Mocks::Page>();
  auto page2 = std::make_unique<Mocks::Page>();

  Slab::ListDescriptor desc1{reinterpret_cast<void*>(page1.get()), 8, 1};
  Slab::ListDescriptor desc2{reinterpret_cast<void*>(page2.get()), 8, 1};

  desc1.AddList(&desc2);

  CHECK(desc1.PrevList() == &desc2);
  CHECK(desc1.NextList() == &desc2);

  CHECK(desc2.PrevList() == &desc1);
  CHECK(desc2.NextList() == &desc1);
}

TEST_CASE("List Descriptors will maintain the correct linkages when size is 3, order is correct") {
  auto page1 = std::make_unique<Mocks::Page>();
  auto page2 = std::make_unique<Mocks::Page>();
  auto page3 = std::make_unique<Mocks::Page>();

  Slab::ListDescriptor desc1{reinterpret_cast<void*>(page1.get()), 8, 1};
  Slab::ListDescriptor desc2{reinterpret_cast<void*>(page2.get()), 8, 1};
  Slab::ListDescriptor desc3{reinterpret_cast<void*>(page3.get()), 8, 1};

  desc1.AddList(&desc2);
  desc1.AddList(&desc3);

  CHECK(desc1.PrevList() == &desc3);
  CHECK(desc1.NextList() == &desc2);

  CHECK(desc2.PrevList() == &desc1);
  CHECK(desc2.NextList() == &desc3);

  CHECK(desc3.PrevList() == &desc2);
  CHECK(desc3.NextList() == &desc1);
}

TEST_CASE("List Descriptors will not add descriptors of other sizes to their linkages"){
  auto page1 = std::make_unique<Mocks::Page>();
  auto page2 = std::make_unique<Mocks::Page>();
  auto page3 = std::make_unique<Mocks::Page>();
  auto page4 = std::make_unique<Mocks::Page>();


  Slab::ListDescriptor desc1{reinterpret_cast<void*>(page1.get()), 8, 1};
  Slab::ListDescriptor desc2{reinterpret_cast<void*>(page2.get()), 16, 1};
  Slab::ListDescriptor desc3{reinterpret_cast<void*>(page3.get()), 24, 1};
  Slab::ListDescriptor desc4{reinterpret_cast<void*>(page4.get()), 32, 1};

  CHECK(desc1.PrevList() == &desc1);
  CHECK(desc1.NextList() == &desc1);

  desc1.AddList(&desc2);

  CHECK(desc1.PrevList() == &desc1);
  CHECK(desc1.NextList() == &desc1);

  desc1.AddList(&desc3);

  CHECK(desc1.PrevList() == &desc1);
  CHECK(desc1.NextList() == &desc1);

  desc1.AddList(&desc4);

  CHECK(desc1.PrevList() == &desc1);
  CHECK(desc1.NextList() == &desc1);

  desc2.AddList(&desc3);

  CHECK(desc2.PrevList() == &desc2);
  CHECK(desc2.NextList() == &desc2);

  desc2.AddList(&desc4);

  CHECK(desc2.PrevList() == &desc2);
  CHECK(desc2.NextList() == &desc2);

  desc3.AddList(&desc4);

  CHECK(desc3.PrevList() == &desc3);
  CHECK(desc3.PrevList() == &desc3);

  CHECK(desc4.NextList() == &desc4);
  CHECK(desc4.NextList() == &desc4);
}

TEST_CASE("ListDescriptors::Remove will remove linkages from the list descriptor"){
  auto page1 = std::make_unique<Mocks::Page>();
  auto page2 = std::make_unique<Mocks::Page>();
  auto page3 = std::make_unique<Mocks::Page>();
  auto page4 = std::make_unique<Mocks::Page>();

  Slab::ListDescriptor desc1{reinterpret_cast<void*>(page1.get()), 8, 1};
  Slab::ListDescriptor desc2{reinterpret_cast<void*>(page2.get()), 8, 1};
  Slab::ListDescriptor desc3{reinterpret_cast<void*>(page3.get()), 8, 1};

  desc1.AddList(&desc2);
  desc1.AddList(&desc3);

  // just checking if the premise is correct:
  // desc1 <-> desc2 <-> desc3 <-> desc1 

  REQUIRE(desc1.PrevList() == &desc3);
  REQUIRE(desc1.NextList() == &desc2);

  REQUIRE(desc2.PrevList() == &desc1);
  REQUIRE(desc2.NextList() == &desc3);

  REQUIRE(desc3.PrevList() == &desc2);
  REQUIRE(desc3.NextList() == &desc1);

  // Actual Testing Code

  desc1.Remove(&desc3);

  CHECK(desc1.PrevList() == &desc2);
  CHECK(desc1.NextList() == &desc2);

  CHECK(desc2.PrevList() == &desc1);
  CHECK(desc2.NextList() == &desc1);

  desc1.Remove(&desc2);

  CHECK(desc1.PrevList() == &desc1);
  CHECK(desc1.NextList() == &desc1);
}


TEST_CASE("List Descriptors cannot remove themselves"){
  auto page1 = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc1{reinterpret_cast<void*>(page1.get()), 8, 1};

  desc1.Remove(&desc1);

  CHECK(desc1.PrevList() == &desc1);
  CHECK(desc1.PrevList() == &desc1);
}
