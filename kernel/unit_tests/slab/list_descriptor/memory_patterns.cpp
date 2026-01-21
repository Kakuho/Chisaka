#include "doctest.h"

#include <cstdlib>
#include <memory>
#include <array>

#include "kheap/slab/list_descriptor.hpp"
#include "mocks/page.hpp"
#include "aii/array.hpp"

//  suite of different memory patterns that can be used to test allocatind and dealllocating from the list descriptor
//  can likely contain a ton of memory patterns, the more the better 
// 
//  can also be repeated with a different sized input, best to test:
//    8 bytes, 16 bytes, 32bytes, 64 bytes, 128 bytes, 256 bytes, 512 bytes, 1024 bytes, 2048 bytes, 4096 bytes
//
//  for now just test 8, 64, 512

using namespace Chisaka;

struct Buffer8{
  const Buffer8& operator=(std::uint64_t src){ val = src; return *this;}
  bool operator==(std::uint64_t src) const{ return val == src;}
  std::uint64_t& Val(){ return val;}
  std::uint64_t val;
};

static_assert(sizeof(Buffer8) == 8);

struct Buffer64{
  const Buffer64& operator=(std::uint64_t src){ b1.val = src; return *this;}
  bool operator==(std::uint64_t src) const{ return b1.val == src;}
  Buffer8 b1;
  Buffer8 b2{0};
  Buffer8 b3{0};
  Buffer8 b4{0};
  Buffer8 b5;
  Buffer8 b6{0};
  Buffer8 b7{0};
  Buffer8 b8{0};
};

static_assert(sizeof(Buffer64) == 64);

struct Buffer512{
  const Buffer512& operator=(std::uint64_t src){ b1.b1.val = src; return *this;}
  bool operator==(std::uint64_t src) const{ return b1.b1.val == src;}
  Buffer64 b1;
  Buffer64 b2;
  Buffer64 b3;
  Buffer64 b4;
  Buffer64 b5;
  Buffer64 b6;
  Buffer64 b7;
  Buffer64 b8;
};

static_assert(sizeof(Buffer512) == 512);

using BufferType= Buffer512;
                                  
TEST_CASE("list descriptor memory pattern: alloc, alloc, dealloc dealloc"){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{page.get(), sizeof(BufferType), 1};
  std::size_t buffers = desc.TotalBuffers();
  desc.SetupLinkage();
  //page->Dump("initial_page.dump");

  BufferType* obj1 = reinterpret_cast<BufferType*>(desc.Allocate());
  BufferType* obj2 = reinterpret_cast<BufferType*>(desc.Allocate());

  *obj1 = 0xDEADBEEF;
  *obj2 = 0xDEADDEAD;

  BufferType* pagemem = reinterpret_cast<BufferType*>(&page->bytes[0]);

  CHECK(pagemem[0] == 0xDEADBEEF);
  CHECK(pagemem[1] == 0xDEADDEAD);
  REQUIRE(desc.NextFreeBuffer() == &pagemem[2]);

  CHECK(buffers == desc.TotalBuffers());
  CHECK(desc.BuffersUsed() == 2);
  CHECK(!desc.Empty());
  CHECK(!desc.Full());

  //page->Dump("page1.dump");

  desc.Deallocate(obj1);
  REQUIRE(desc.NextFreeBuffer() == obj1);

  //page->Dump("page2.dump");

  desc.Deallocate(obj2);
  //page->Dump("page3.dump");
  REQUIRE(desc.NextFreeBuffer() == obj1);

  CHECK(pagemem[0] != 0xDEADBEEF);
  CHECK(pagemem[1] != 0xDEADDEAD);
  CHECK(desc.Empty());
  CHECK(desc.BuffersUsed() == 0);

  //page->Dump("page4.dump");

  // now we perform an exhaustion check to see if it can allocate TotalBuffers amount of buffers, 
  // just to check the linkages

  std::size_t count = 0;
  BufferType* leaky = nullptr;
  for(std::size_t i = 0; i < desc.TotalBuffers(); i++){
    leaky = reinterpret_cast<BufferType*>(desc.Allocate());
    count++;
  }
  BufferType* isNull = reinterpret_cast<BufferType*>(desc.Allocate());

  CHECK(isNull == nullptr);
  CHECK(desc.NextFreeBuffer() == nullptr);
  CHECK(desc.BuffersUsed() == desc.TotalBuffers());
  CHECK(desc.Full());
  CHECK(!desc.HasSpace());
  CHECK(count == desc.TotalBuffers());
}

TEST_CASE("list descriptor memory pattern #1: alloc, alloc, alloc, dealloc, dealloc, dealloc FIFO"){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{page.get(), sizeof(BufferType), 1};
  std::size_t buffers = desc.TotalBuffers();
  desc.SetupLinkage();

  BufferType* obj1 = reinterpret_cast<BufferType*>(desc.Allocate());
  BufferType* obj2 = reinterpret_cast<BufferType*>(desc.Allocate());
  BufferType* obj3 = reinterpret_cast<BufferType*>(desc.Allocate());

  *obj1 = 0xDEADBEEF;
  *obj2 = 0xDEADDEAD;
  *obj3 = 0xBEEFBEEF;

  BufferType* pagemem = reinterpret_cast<BufferType*>(&page->bytes[0]);

  CHECK(pagemem[0] == 0xDEADBEEF);
  CHECK(pagemem[1] == 0xDEADDEAD);
  CHECK(pagemem[2] == 0xBEEFBEEF);

  CHECK(buffers == desc.TotalBuffers());
  CHECK(desc.BuffersUsed() == 3);
  CHECK(!desc.Empty());
  CHECK(!desc.Full());

  desc.Deallocate(obj1);
  desc.Deallocate(obj2);
  desc.Deallocate(obj3);

  CHECK(pagemem[0] != 0xDEADBEEF);
  CHECK(pagemem[1] != 0xDEADDEAD);
  CHECK(pagemem[2] != 0xBEEFBEEF);
  CHECK(desc.Empty());

  // now we perform an exhaustion check to see if it can allocate TotalBuffers amount of buffers, 
  // just to check the linkages

  std::size_t count = 0;
  BufferType* leaky = nullptr;
  for(auto i = 0; i < desc.TotalBuffers() ; i++){
    leaky = reinterpret_cast<BufferType*>(desc.Allocate());
    count++;
  }
  BufferType* isNull = reinterpret_cast<BufferType*>(desc.Allocate());

  REQUIRE(isNull == nullptr);
  REQUIRE(desc.NextFreeBuffer() == nullptr);
  REQUIRE(desc.BuffersUsed() == desc.TotalBuffers());
  REQUIRE(desc.Full());
  REQUIRE(!desc.HasSpace());
  CHECK(count == desc.TotalBuffers());
}

TEST_CASE("list descriptor memory pattern #2 : alloc, alloc, alloc, dealloc, dealloc, dealloc LIFO"){

}

TEST_CASE("list descriptor memory pattern #3 (dealloc after exhaust): exhaust, dealloc, alloc"){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{page.get(), sizeof(BufferType), 1};
  std::size_t buffers = desc.TotalBuffers();
  desc.SetupLinkage();

  // exhaust the thing first

  BufferType* leaky = nullptr;
  for(auto i = 0; i < desc.TotalBuffers() ; i++){
    leaky = reinterpret_cast<BufferType*>(desc.Allocate());
  }
  BufferType* isNull = reinterpret_cast<BufferType*>(desc.Allocate());
  CHECK(isNull == nullptr);

  CHECK(desc.NextFreeBuffer() == nullptr);
  CHECK(desc.BuffersUsed() == desc.TotalBuffers());
  CHECK(desc.Full());
  CHECK(!desc.HasSpace());

  desc.Deallocate(leaky);
  CHECK(desc.BuffersUsed() == desc.TotalBuffers() -1);
  CHECK(desc.NextFreeBuffer() == leaky);
  CHECK(desc.HasSpace());

  leaky = reinterpret_cast<BufferType*>(desc.Allocate());
  CHECK(isNull == nullptr);
  CHECK(desc.NextFreeBuffer() == nullptr);
  CHECK(desc.BuffersUsed() == desc.TotalBuffers());
  CHECK(desc.Full());
  CHECK(!desc.HasSpace());
}

TEST_CASE("list descriptor memory pattern #4 (dealloc after exhaust, longer): exhaust, dealloc, alloc, dealloc dealloc, alloc, alloc"){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{page.get(), sizeof(BufferType), 1};
  std::size_t buffers = desc.TotalBuffers();
  desc.SetupLinkage();

  // exhaust the thing first

  std::array<BufferType*, 2> objects;
  BufferType*& leaky = objects[0];
  leaky = nullptr;
  for(auto i = 0; i < desc.TotalBuffers() ; i++){
    objects[1] = leaky;
    leaky = reinterpret_cast<BufferType*>(desc.Allocate());
  }
  REQUIRE(desc.NextFreeBuffer() == nullptr);
  REQUIRE(desc.Full());

  desc.Deallocate(leaky);
  REQUIRE(desc.NextFreeBuffer() != nullptr);
  REQUIRE(desc.HasSpace());

  leaky = reinterpret_cast<BufferType*>(desc.Allocate());
  REQUIRE(desc.NextFreeBuffer() == nullptr);
  REQUIRE(desc.Full());

  // dealloc dealloc
  
  desc.Deallocate(objects[0]);
  desc.Deallocate(objects[1]);
  REQUIRE(desc.NextFreeBuffer() != nullptr);
  REQUIRE(desc.HasSpace());

  // alloc alloc  
  objects[0] =  reinterpret_cast<BufferType*>(desc.Allocate());
  objects[1] =  reinterpret_cast<BufferType*>(desc.Allocate());

  CHECK(desc.NextFreeBuffer() == nullptr);
  CHECK(desc.Full());
}

TEST_CASE("list descriptor memory pattern (exhaust and free-all): exhaust, free-all"){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{page.get(), sizeof(BufferType), 1};
  std::size_t buffers = desc.TotalBuffers();
  desc.SetupLinkage();

  constexpr std::size_t totalBuffers= 0x1000 / sizeof(BufferType);

  // exhaust the thing first

  std::array<BufferType*, totalBuffers> objects;
  for(auto i = 0; i < desc.TotalBuffers() ; i++){
    objects[i] = reinterpret_cast<BufferType*>(desc.Allocate());
    *objects[i] = i;
  }
  REQUIRE(desc.NextFreeBuffer() == nullptr);
  REQUIRE(desc.Full());
  REQUIRE(desc.BuffersUsed() == desc.TotalBuffers());
  //page->Dump("exhauts_freeall.dump");

  // now we free all
  for(auto i = desc.TotalBuffers() -1; i >= 0 ; i--){
    desc.Deallocate(objects[i]);
    //page->Dump("exhauts_freeall.dump");
  }
  CHECK(desc.NextFreeBuffer() != nullptr);
  CHECK(desc.Empty());
  CHECK(desc.BuffersUsed() == 0);
}

TEST_CASE("list descriptor memory pattern (exhaust and free-all): exhaust, free-all, exhaust, free-all"){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{page.get(), sizeof(BufferType), 1};
  std::size_t buffers = desc.TotalBuffers();
  desc.SetupLinkage();

  constexpr std::size_t totalBuffers= 0x1000 / sizeof(BufferType);

  // exhaust the thing first

  std::array<BufferType*, totalBuffers> objects;
  for(auto i = 0; i < desc.TotalBuffers() ; i++){
    objects[i] = reinterpret_cast<BufferType*>(desc.Allocate());
    *objects[i] = i;
  }
  REQUIRE(desc.NextFreeBuffer() == nullptr);
  REQUIRE(desc.Full());
  REQUIRE(desc.BuffersUsed() == desc.TotalBuffers());

  // now we free all
  for(auto i = desc.TotalBuffers() -1; i >= 0 ; i--){
    desc.Deallocate(objects[i]);
  }
  CHECK(desc.NextFreeBuffer() != nullptr);
  CHECK(desc.Empty());
  CHECK(desc.BuffersUsed() == 0);

  // exhaust again
  for(auto i = 0; i < desc.TotalBuffers() ; i++){
    objects[i] = reinterpret_cast<BufferType*>(desc.Allocate());
    *objects[i] = i;
  }
  REQUIRE(desc.NextFreeBuffer() == nullptr);
  REQUIRE(desc.Full());
  REQUIRE(desc.BuffersUsed() == desc.TotalBuffers());

  // free it agian, but this time from the start
  for(auto i = 0; i < desc.TotalBuffers(); i++){
    desc.Deallocate(objects[i]);
  }
  CHECK(desc.NextFreeBuffer() != nullptr);
  CHECK(desc.Empty());
  CHECK(desc.BuffersUsed() == 0);
}


TEST_CASE("list descriptor memory pattern (fragmentation) : in-use free in-use free in-use"){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{page.get(), sizeof(BufferType), 1};
  std::size_t buffers = desc.TotalBuffers();
  desc.SetupLinkage();
  BufferType* pagemem = reinterpret_cast<BufferType*>(&page->bytes[0]);

  BufferType* p1 = reinterpret_cast<BufferType*>(desc.Allocate());
  BufferType* p2 = reinterpret_cast<BufferType*>(desc.Allocate());
  BufferType* p3 = reinterpret_cast<BufferType*>(desc.Allocate());
  BufferType* p4 = reinterpret_cast<BufferType*>(desc.Allocate());
  BufferType* p5 = reinterpret_cast<BufferType*>(desc.Allocate());

  *p1 = 0xDEADBEEF;
  *p2 = 0xDEADBEEF;     // hole
  *p3 = 0xDEADBEEF;
  *p4 = 0xDEADBEEF;     // hole
  *p5 = 0xDEADBEEF;

  desc.Deallocate(p1);
  desc.Deallocate(p3);
  desc.Deallocate(p5);

  CHECK(pagemem[0] != 0xDEADBEEF);
  CHECK(pagemem[1] == 0xDEADBEEF);
  CHECK(pagemem[2] != 0xDEADBEEF);
  CHECK(pagemem[3] == 0xDEADBEEF);
  CHECK(pagemem[4] != 0xDEADBEEF);
  CHECK(desc.BuffersUsed() == 2);

  // exhaustion check

  std::size_t count = 0;
  BufferType* leaky = nullptr;
  for(std::size_t i = 0; i < desc.TotalBuffers() - 2; i++){
    leaky = reinterpret_cast<BufferType*>(desc.Allocate());
    count++;
  }
  BufferType* isNull = reinterpret_cast<BufferType*>(desc.Allocate());
  CHECK(isNull == nullptr);
  CHECK(desc.NextFreeBuffer() == nullptr);
  CHECK(desc.BuffersUsed() == desc.TotalBuffers());
  CHECK(desc.Full());
  CHECK(!desc.HasSpace());
  CHECK(count == desc.TotalBuffers() - 2);
}

TEST_CASE("list descriptor memory pattern #7: alloc, alloc, dealloc, alloc, exhaust, dealloc, alloc, dealloc"){

}

TEST_CASE("list descriptor memory pattern #8 (repeated exhaustion): exhaust, exhaust, exhaust, exhaust"){

}
