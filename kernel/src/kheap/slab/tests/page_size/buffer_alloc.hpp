#pragma once

#include "./../../list_descriptor.hpp"
#include "./../../buffer.hpp"

#include "memory/heap/allocator.hpp"
#include "memory/page/freelist.hpp"
#include "memory/memory_map/memory_map_descriptor.hpp"

namespace Mem::Heap::Test::PageSize::Buffer{

  inline static constexpr std::uintptr_t BASE_ADDR1 = 0x55000;
  inline static constexpr std::uintptr_t BASE_ADDR2 = 0x56000;
  inline static constexpr std::uintptr_t BASE_ADDR3 = 0x57000;
  inline static constexpr std::uintptr_t BASE_ADDR4 = 0x58000;
  inline static constexpr std::uintptr_t BASE_ADDR5 = 0x59000;
  inline static constexpr std::uintptr_t BASE_ADDR6 = 0x60000;
  inline static constexpr std::uintptr_t BASE_ADDR7 = 0x61000;
  inline static constexpr std::uintptr_t BASE_ADDR8 = 0x62000;

  inline static constexpr std::uintptr_t VBASE_ADDR1 = 0xFFFF800000055000;

  inline static constexpr std::uint64_t BUFFER_SIZE = 0x1000;
  inline static constexpr std::uint16_t PAGE_SIZE = 0x1000;

  void Allocate1(); void Allocate2();
  void AllocateFromEmpty1(); void AllocateFromEmpty2();

  inline void Allocate1(){
    // Simple check to see if the initial free list gets moved to the full list
    // A page will exhaust a list descriptor on use
    kout << intmode::hex << "Base: " << BASE_ADDR1 << '\n';

    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR1), BUFFER_SIZE, 1};
    desc.SetupLinkage();

    Heap::Buffer buf{BUFFER_SIZE};
    buf.AddList(&desc);

    kassert(buf.FreeListHead() == &desc);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    void* nextfree = desc.NextFreeBuffer();
    void* allocd = buf.Allocate();

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == &desc);

    kassert(allocd == nextfree);

    kout << "Allocate1() Success" << '\n';
  }

  inline void Allocate2(){
    // Premise:
    //  Have a full list, and a free list, make sure
    //  that the next allocated belongs to the free list descriptor
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), PAGE_SIZE, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), PAGE_SIZE, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), PAGE_SIZE, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), PAGE_SIZE, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();

    // d1 d3 shall be full, d2 d4 shall be free
    desc1.Allocate();
    desc3.Allocate();

    Heap::Buffer buf{PAGE_SIZE};

    // this needs to come afterwards to ensure correct calculations
    buf.AddList(&desc1);
    buf.AddList(&desc2);
    buf.AddList(&desc3);
    buf.AddList(&desc4);

    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.FreeListHead()->NextList() == &desc4);
    kassert(buf.FreeListHead()->PrevList() == &desc4);

    kassert(buf.FullListHead() == &desc1);
    kassert(buf.FullListHead()->NextList() == &desc3);
    kassert(buf.FullListHead()->PrevList() == &desc3);

    kassert(buf.PartialListHead() == nullptr);

    // the next allocated should be from desc2
    void* nextfree = desc2.NextFreeBuffer();
    void* allocd = buf.Allocate();

    kassert(allocd == nextfree);

    // Ensure correct list linkages
    kassert(buf.FreeListHead() == &desc4);
    kassert(buf.FreeListHead()->NextList() == &desc4);
    kassert(buf.FreeListHead()->PrevList() == &desc4);

    kassert(buf.FullListHead() == &desc1);
    kassert(buf.FullListHead()->NextList() == &desc3);
    kassert(buf.FullListHead()->NextList()->NextList() == &desc2);
    kassert(buf.FullListHead()->PrevList() == &desc2);
    kassert(buf.FullListHead()->PrevList()->PrevList() == &desc3);

    kassert(buf.PartialListHead() == nullptr);
    kout << "TryAllocate_Mock2 Success!" << '\n';
  }

  inline void AllocateFromEmpty1(){
    kout << intmode::hex << "Base: " << BASE_ADDR1 << '\n';

    Mem::MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);
    Heap::Allocator::Initialise();

    Heap::Buffer buf{PAGE_SIZE};
    void* obj1 = buf.Allocate();
    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.FullListHead() != nullptr);

    buf.Deallocate(obj1);
    kassert(buf.FreeListHead() != nullptr);
    kassert(buf.FullListHead() == nullptr);

    kout << "AllocateFromEmtpy1() Success!" << '\n';
  }

  inline void AllocateFromEmpty2(){
    kout << intmode::hex << "Base: " << BASE_ADDR1 << '\n';

    Mem::MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);
    Heap::Allocator::Initialise();

    Heap::Buffer buf{PAGE_SIZE};
    void* obj1 = buf.Allocate();
    void* obj2 = buf.Allocate();
    void* obj3 = buf.Allocate();
    void* obj4 = buf.Allocate();

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.FullListHead() != nullptr);

    buf.Deallocate(obj2);
    kassert(buf.FreeListHead() != nullptr);

    buf.Deallocate(obj4);
    kassert(buf.FreeListHead() != nullptr);
    kassert(buf.FreeListHead()->NextList() != nullptr);
    kassert(buf.FreeListHead()->NextList() != buf.FreeListHead());
    kassert(buf.FreeListHead()->NextList() == buf.FreeListHead()->PrevList());

    kassert(buf.FullListHead() != nullptr);
    kassert(buf.FullListHead()->NextList() != nullptr);
    kassert(buf.FullListHead()->NextList() != buf.FullListHead());
    kassert(buf.FullListHead()->NextList() == buf.FullListHead()->PrevList());

    kout << "AllocateFromEmtpy2() Success!" << '\n';
  }

}
