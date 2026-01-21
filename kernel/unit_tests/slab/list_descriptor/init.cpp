#include "doctest.h"

#include <iostream>
#include <memory>

#include "mocks/page.hpp"
#include "kheap/slab/list_descriptor.hpp"

using namespace Chisaka;

static bool PageEquals(Mocks::Page& page, std::uint8_t val){
  for(std::size_t i = 0; i < 0x1000; i++){
    if(page.bytes[i] != val){
      return false;
    }
  }
  return true;
}

static void PrintPage(Mocks::Page& page){
  for(std::size_t i = 0; i < 1000; i++){
    std::cout << std::hex << page.bytes[i] << ' ';
    if(i % 8 == 0){
      std::cout << '\n';
    }
  }
}

TEST_CASE("List Descriptors will correctly poison itself") {
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{reinterpret_cast<void*>(page.get()), 8, 1};
  desc.PoisonFreeArea(0xBE);
  CHECK(PageEquals(*page, 0xBE));
}

TEST_CASE("List Descriptors correctly initialises its data members(buffersize=8, pages=1)") {
  SUBCASE("Buffer Size 8, Page = 1"){
    auto page1 = std::make_unique<Mocks::Page>();
    Slab::ListDescriptor desc1{reinterpret_cast<void*>(page1.get()), 8, 1};

    CHECK(desc1.BufferSize() == 8);
    CHECK(desc1.Pages() == 1);
    CHECK(desc1.TotalBuffers() == 0x1000/8);
    CHECK(desc1.BuffersUsed() == 0);
    CHECK(desc1.Base() == reinterpret_cast<std::uintptr_t>(page1.get()));
    CHECK(desc1.Limit() == reinterpret_cast<std::uintptr_t>(page1.get()) + 0x1000);
    CHECK(desc1.InitialFreeSpace() == 0x1000);
    CHECK(desc1.SpaceAfterAllocation() == 0x1000 - (0x1000/8) * 8);
    CHECK(desc1.Full() == false);
    CHECK(desc1.Empty() == true);
    CHECK(desc1.HasSpace() == true);
  }
  SUBCASE("BufferSize 400, Page = 1"){
    auto page = std::make_unique<Mocks::Page>();
    constexpr std::uint16_t BUFFERSIZE = 400;
    Slab::ListDescriptor desc{reinterpret_cast<void*>(page.get()), BUFFERSIZE, 1};

    CHECK(desc.BufferSize() == BUFFERSIZE);
    CHECK(desc.Pages() == 1);
    CHECK(desc.TotalBuffers() == 0x1000/BUFFERSIZE);
    CHECK(desc.BuffersUsed() == 0);
    CHECK(desc.Base() == reinterpret_cast<std::uintptr_t>(page.get()));
    CHECK(desc.Limit() == reinterpret_cast<std::uintptr_t>(page.get()) + 0x1000);
    CHECK(desc.InitialFreeSpace() == 0x1000);
    CHECK(desc.SpaceAfterAllocation() == 0x1000 - (0x1000/BUFFERSIZE) * BUFFERSIZE);
    CHECK(desc.Full() == false);
    CHECK(desc.Empty() == true);
    CHECK(desc.HasSpace() == true);
  }
}

TEST_CASE("List Descriptors with buffer size less than 8 will be initialised with size 8") {
  SUBCASE("Buffer Size 1, Page = 1"){
    auto page1 = std::make_unique<Mocks::Page>();
    constexpr std::uint16_t BUFFERSIZE = 1;
    Slab::ListDescriptor desc1{reinterpret_cast<void*>(page1.get()), BUFFERSIZE, 1};

    CHECK(desc1.BufferSize() == 8);
    CHECK(desc1.Pages() == 1);
    CHECK(desc1.TotalBuffers() == 0x1000/8);
    CHECK(desc1.BuffersUsed() == 0);
    CHECK(desc1.Base() == reinterpret_cast<std::uintptr_t>(page1.get()));
    CHECK(desc1.Limit() == reinterpret_cast<std::uintptr_t>(page1.get()) + 0x1000);
    CHECK(desc1.InitialFreeSpace() == 0x1000);
    CHECK(desc1.SpaceAfterAllocation() == 0x1000 - (0x1000/8) * 8);
    CHECK(desc1.Full() == false);
    CHECK(desc1.Empty() == true);
    CHECK(desc1.HasSpace() == true);
  }

  SUBCASE("BufferSize 4, Page = 1"){
    auto page = std::make_unique<Mocks::Page>();
    constexpr std::uint16_t BUFFERSIZE = 4;
    Slab::ListDescriptor desc{reinterpret_cast<void*>(page.get()), BUFFERSIZE, 1};

    CHECK(desc.BufferSize() == 8);
    CHECK(desc.Pages() == 1);
    CHECK(desc.TotalBuffers() == 0x1000/8);
    CHECK(desc.BuffersUsed() == 0);
    CHECK(desc.Base() == reinterpret_cast<std::uintptr_t>(page.get()));
    CHECK(desc.Limit() == reinterpret_cast<std::uintptr_t>(page.get()) + 0x1000);
    CHECK(desc.InitialFreeSpace() == 0x1000);
    CHECK(desc.SpaceAfterAllocation() == 0x1000 - (0x1000/8) * 8);
    CHECK(desc.Full() == false);
    CHECK(desc.Empty() == true);
    CHECK(desc.HasSpace() == true);
  }
}
