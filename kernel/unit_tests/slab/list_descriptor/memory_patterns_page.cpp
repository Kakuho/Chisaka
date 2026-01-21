#include "doctest.h"

#include <cstdlib>
#include <memory>
#include <array>

#include "kheap/slab/list_descriptor.hpp"
#include "mocks/page.hpp"
#include "aii/array.hpp"

//  tests for page sized memory buffers

using namespace Chisaka;

struct BufferPage{
  const BufferPage& operator=(std::uint64_t src){ buffer[0] = src; return *this;}
  bool operator==(std::uint64_t src) const{ return buffer[0] == src;}
  std::array<std::uint64_t, 0x200> buffer;
};

static_assert(sizeof(BufferPage) == 0x1000);

using BufferType= BufferPage;
                                  
TEST_CASE("list descriptor page sized memory pattern: alloc"){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{page.get(), sizeof(BufferType), 1};
  desc.SetupLinkage();

  BufferType* obj1 = reinterpret_cast<BufferType*>(desc.Allocate());
  *obj1 = 0xDEADBEEF;

  BufferType* pagemem = reinterpret_cast<BufferType*>(&page->bytes[0]);

  CHECK(pagemem[0] == 0xDEADBEEF);
  CHECK(desc.NextFreeBuffer() == nullptr);
  CHECK(desc.BuffersUsed() == desc.TotalBuffers());
  CHECK(desc.Full());
  CHECK(!desc.HasSpace());
}

TEST_CASE("list descriptor page sized memory pattern: alloc, alloc"){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{page.get(), sizeof(BufferType), 1};
  desc.SetupLinkage();

  BufferType* obj1 = reinterpret_cast<BufferType*>(desc.Allocate());
  *obj1 = 0xDEADBEEF;

  BufferType* obj2 = reinterpret_cast<BufferType*>(desc.Allocate());
  CHECK(obj2 == nullptr);

  BufferType* pagemem = reinterpret_cast<BufferType*>(&page->bytes[0]);

  CHECK(pagemem[0] == 0xDEADBEEF);
  CHECK(desc.NextFreeBuffer() == nullptr);
  CHECK(desc.BuffersUsed() == desc.TotalBuffers());
  CHECK(desc.Full());
  CHECK(!desc.HasSpace());

}

TEST_CASE("list descriptor page sized memory pattern: alloc, dealloc"){
  auto page = std::make_unique<Mocks::Page>();
  Slab::ListDescriptor desc{page.get(), sizeof(BufferType), 1};
  desc.SetupLinkage();
  BufferType* pagemem = reinterpret_cast<BufferType*>(&page->bytes[0]);

  BufferType* obj1 = reinterpret_cast<BufferType*>(desc.Allocate());
  *obj1 = 0xDEADBEEF;
  CHECK(pagemem[0] == 0xDEADBEEF);

  desc.Deallocate(obj1);
  CHECK(desc.NextFreeBuffer() != nullptr);
  CHECK(desc.BuffersUsed() == 0);
  CHECK(desc.Empty());
  CHECK(desc.HasSpace());
}

TEST_CASE("list descriptor page sized memory pattern: (alloc, dealloc) x 10"){
  CHECK(false);
}

TEST_CASE("list descriptor page sized memory pattern: alloc dealloc dealloc"){
  CHECK(false);
}
