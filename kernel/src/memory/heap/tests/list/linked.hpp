#pragma once

#include <cstdint>

#include "./../../list_descriptor.hpp"

namespace Mem::Heap::Test::List::Linked{

  static constexpr std::uintptr_t BASE_ADDR = 0x55000;
  static constexpr std::uintptr_t VBASE_ADDR = 0xFFFF800000055000;
  static constexpr std::uint16_t PAGE_SIZE = 0x1000;

  void TrySize1();

  void TryAdd();

  void TryAddDifferentSize();

  void TryRemove();

  inline void TrySize1(){
    // TrySize1()
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 8, 1};
    desc.PoisonFreeArea(0xBE);

    kassert(desc.PrevList() == &desc);
    kassert(desc.NextList() == &desc);
  }

  inline void TryAdd(){
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR), 8, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR+0x1000), 8, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR+0x2000), 8, 1};

    kassert(desc1.PrevList() == &desc1);
    kassert(desc1.NextList() == &desc1);

    desc1.AddList(&desc2);

    kassert(desc1.PrevList() == &desc2);
    kassert(desc1.NextList() == &desc2);

    kassert(desc2.PrevList() == &desc1);
    kassert(desc2.NextList() == &desc1);

    desc1.AddList(&desc3);

    kassert(desc1.PrevList() == &desc3);
    kassert(desc1.NextList() == &desc2);

    kassert(desc2.PrevList() == &desc1);
    kassert(desc2.NextList() == &desc3);

    kassert(desc3.PrevList() == &desc2);
    kassert(desc3.NextList() == &desc1);
  }

  inline void TryAddDifferentSize(){
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR), 8, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR+0x1000), 0x100, 1};

    kassert(desc1.PrevList() == &desc1);
    kassert(desc1.NextList() == &desc1);

    desc1.AddList(&desc2);

    kassert(desc1.PrevList() == &desc1);
    kassert(desc1.NextList() == &desc1);
  }

  inline void TryRemove(){
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR), 8, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR+0x1000), 8, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR+0x2000), 8, 1};

    desc1.AddList(&desc2);
    desc1.AddList(&desc3);

    kassert(desc1.PrevList() == &desc3);
    kassert(desc1.NextList() == &desc2);

    kassert(desc2.PrevList() == &desc1);
    kassert(desc2.NextList() == &desc3);

    kassert(desc3.PrevList() == &desc2);
    kassert(desc3.NextList() == &desc1);

    desc1.Remove(&desc3);

    kassert(desc1.PrevList() == &desc2);
    kassert(desc1.NextList() == &desc2);

    kassert(desc2.PrevList() == &desc1);
    kassert(desc2.NextList() == &desc1);

    desc1.Remove(&desc2);

    kassert(desc1.PrevList() == &desc1);
    kassert(desc1.NextList() == &desc1);

    desc1.Remove(&desc1);

  }
}
