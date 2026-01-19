#pragma once

#include "./../../list_descriptor.hpp"

namespace Mem::Heap::Test::PageSize::List{

  inline static constexpr std::uintptr_t BASE_ADDR = 0x55000;
  inline static constexpr std::uintptr_t VBASE_ADDR = 0xFFFF800000055000;

  inline static constexpr std::uint64_t BUFFER_SIZE = 0x1000;
  inline static constexpr std::uint16_t PAGE_SIZE = 0x1000;

  void Init();
  void Allocate();
  void Deallocate();

  inline void Init(){
    kout << intmode::hex << "Phys Base: " << BASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), BUFFER_SIZE, 1};
    desc.PoisonFreeArea(0xBB);

    kassert(desc.BufferSize() == BUFFER_SIZE);
    kassert(desc.Pages() == 1);
    kassert(desc.Base() == BASE_ADDR);
    kassert(desc.Limit() == BASE_ADDR + desc.Pages() * PAGE_SIZE);

    kout << "List Descriptor Initial Space: " << desc.InitialFreeSpace() << '\n';
    kassert(desc.InitialFreeSpace() == PAGE_SIZE);

    kout << "List Total Buffers: " << desc.TotalBuffers() << '\n';
    kassert(desc.TotalBuffers() == PAGE_SIZE/BUFFER_SIZE);

    kout << "Space After Allocation: " << desc.SpaceAfterAllocation() << '\n';
    kassert(desc.SpaceAfterAllocation() == 0); // 8 bytes fit perfectly within 0x1000

    kassert(desc.Empty() == true);
    kassert(desc.Full() == false);
    kassert(desc.HasSpace() == true);
    kout << "Init() Success!" << '\n';
  }

  inline void Allocate(){
    kout << intmode::hex << "Phys Base: " << BASE_ADDR << '\n';

    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), BUFFER_SIZE, 1};
    desc.PoisonFreeArea(0xBE);
    desc.SetupLinkage();

    std::uint64_t* obj1 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj1 = 0xDEADBEEF;

    std::uint64_t* isNull = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    kassert(isNull == nullptr);

    kassert(desc.NextFreeBuffer() == nullptr);
    kassert(desc.BuffersUsed() == desc.TotalBuffers());
    kassert(desc.Full());
    kassert(!desc.HasSpace());
    kout << "Allocate() Success!" << '\n';
  }

  inline void Deallocate(){
    kout << intmode::hex << "Phys Base: " << BASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 0x1000, 1};
    desc.PoisonFreeArea(0xBE);
    desc.SetupLinkage();
    kassert(desc.BuffersUsed() == 0);

    std::uint64_t* obj1 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj1 = 0xDEADBEEF;
    kassert(desc.NextFreeBuffer() == nullptr);
    kassert(desc.BuffersUsed() == 1);

    desc.Deallocate(obj1);
    kassert(desc.NextFreeBuffer() == obj1);
    kassert(desc.BuffersUsed() == 0);

    kout << "Deallocate() Success!" << '\n';
  }
}
