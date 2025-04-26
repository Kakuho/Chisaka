#pragma once

#include <cstdint>

#include "./../../list_descriptor.hpp"

namespace Mem::Heap::Test::List::Alloc{

  static constexpr std::uintptr_t BASE_ADDR = 0x55000;
  static constexpr std::uintptr_t VBASE_ADDR = 0xFFFF800000055000;
  static constexpr std::uint16_t PAGE_SIZE = 0x1000;

  void TryMetadata1();
  void TryMetadata2();

  void TryLinkage1();
  void TryLinkage2();

  void TryAllocate1();
  void TryAllocate2();

  void TryExhaust1();
  void TryExhaust2();

  void TryAllocateStress();     // how would you even stress test an allocator?
                                
  void TryObjectRange();

  void TryDeallocate1();
  void TryDeallocate2();

  void TryDoubleDealloc1();
  void TryDoubleDealloc2();

  inline void TryMetadata1(){
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 8, 1};
    desc.PoisonFreeArea(0xBE);

    kassert(desc.BufferSize() == 8);
    kassert(desc.Pages() == 1);
    kassert(desc.Base() == BASE_ADDR);
    kassert(desc.Limit() == BASE_ADDR + desc.Pages() * PAGE_SIZE);

    kout << "List Descriptor Initial Space: " << desc.InitialFreeSpace() << '\n';
    kassert(desc.InitialFreeSpace() == 0x1000);

    kout << "List Total Buffers: " << desc.TotalBuffers() << '\n';
    kassert(desc.TotalBuffers() == 0x1000/8);

    kout << "Space After Allocation: " << desc.SpaceAfterAllocation() << '\n';
    kassert(desc.SpaceAfterAllocation() == 0); // 8 bytes fit perfectly within 0x1000


    kassert(desc.Empty() == true);
    kassert(desc.Full() == false);
    kassert(desc.HasSpace() == true);
    kout << "TryMetadata1() Sucess!" << '\n';
  }

  inline void TryMetadata2(){
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 64, 2};

    desc.PoisonFreeArea(0xCF);

    kassert(desc.BufferSize() == 64);
    kassert(desc.Pages() == 2);
    kassert(desc.Base() == BASE_ADDR);
    kassert(desc.Limit() == BASE_ADDR + desc.Pages() * PAGE_SIZE);

    kout << "List Descriptor Initial Space: " << desc.InitialFreeSpace() << '\n';
    kassert(desc.InitialFreeSpace() == 0x2000);

    kout << "List Total Buffers: " << desc.TotalBuffers() << '\n';
    kassert(desc.TotalBuffers() == 0x2000/64);

    kout << "Space After Allocation: " << desc.SpaceAfterAllocation() << '\n';
    kassert(desc.SpaceAfterAllocation() == 0); // 8 bytes fit perfectly within 0x1000

    kassert(desc.Empty() == true);
    kassert(desc.Full() == false);
    kassert(desc.HasSpace() == true);
    kout << "TryMetadata2() Success!" << '\n';
  }

  inline void TryLinkage1(){
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 8, 1};
    desc.PoisonFreeArea(0xBE);
    desc.SetupLinkage();
  }

  inline void TryLinkage2(){
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';

    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 64, 2};
    desc.PoisonFreeArea(0x00);
    desc.SetupLinkage();
  }

  inline void TryAllocate1(){
    kout << intmode::hex 
         << "Base: " << BASE_ADDR << '\n'
         << "Virt Base: " << VBASE_ADDR << '\n';

    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 8, 1};
    desc.PoisonFreeArea(0xBE);
    desc.SetupLinkage();

    std::uint64_t* obj1 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj1 = 0xDEADBEEF;

    std::uint64_t* obj2 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj2 = 0;

    std::uint64_t* obj3 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj3 = 0x0;

    std::uint64_t* obj4 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj4 = 0xABBAEEABBA;

  }

  inline void TryExhaust1(){
    // Premise: What happense when the allocation gets exhausted?
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 8, 1};
    desc.PoisonFreeArea(0xBE);
    desc.SetupLinkage();
    std::uint64_t* leaky = nullptr;
    for(auto i = 0; i < desc.TotalBuffers() ; i++){
      leaky = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    }
    // descriptor is exhausted
    std::uint64_t* isNull = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    kassert(isNull == nullptr);

    kassert(desc.NextFreeBuffer() == nullptr);
    kassert(desc.BuffersUsed() == desc.TotalBuffers());
    kassert(desc.Full());
    kassert(!desc.HasSpace());
    kout << "TryExhaust1() Success!" << '\n';

  }

  inline void TryExhaust2(){
    // Premise: What happense when the allocation gets exhausted?
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 64, 2};
    desc.PoisonFreeArea(0xBE);
    desc.SetupLinkage();
    std::uint64_t* leaky = nullptr;
    for(auto i = 0; i < desc.TotalBuffers() ; i++){
      leaky = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    }
    // descriptor is exhausted
    std::uint64_t* isNull = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    kassert(isNull == nullptr);

    kassert(desc.NextFreeBuffer() == nullptr);
    kassert(desc.BuffersUsed() == desc.TotalBuffers());
    kassert(desc.Full());
    kassert(!desc.HasSpace());
    kout << "TryExhaust2() Success!" << '\n';
  }

  inline void TryObjectRange(){
    // TryObjectRange()
    kout << intmode::hex 
         << "Base: " << BASE_ADDR << '\n'
         << "Virt Base: " << VBASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 8, 1};
    desc.PoisonFreeArea(0xBE);
    desc.SetupLinkage();

    std::uint64_t* obj1 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    std::uint64_t* obj2 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    std::uint64_t* obj3 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    std::uint64_t* obj4 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    std::uint64_t* obj5 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    std::uint64_t* obj6 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    std::uint64_t* obj7 = reinterpret_cast<std::uint64_t*>(desc.Allocate());

    kassert(desc.ContainsObject(obj1));
    kassert(desc.ContainsObject(obj2));
    kassert(desc.ContainsObject(obj3));
    kassert(desc.ContainsObject(obj4));
    kassert(desc.ContainsObject(obj5));
    kassert(desc.ContainsObject(obj6));
    kassert(desc.ContainsObject(obj7));

    // False
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR - 100)));
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR - 50)));
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR - 20)));
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR - 10)));
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR - 8)));
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR - 1)));

    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR + PAGE_SIZE + 100)));
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR + PAGE_SIZE + 50)));
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR + PAGE_SIZE + 20)));
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR + PAGE_SIZE + 10)));
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR + PAGE_SIZE + 8)));
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR + PAGE_SIZE + 1)));
    kassert(!desc.ContainsObject(reinterpret_cast<void*>(BASE_ADDR + PAGE_SIZE)));

    kout << "TryObjectRange() Success!" << '\n';
  }

  inline void TryDeallocate1(){
    // TryDeallocate1()
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 8, 1};
    desc.PoisonFreeArea(0xBE);
    desc.SetupLinkage();

    std::uint64_t* obj1 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj1 = 0xDEADBEEF;

    std::uint64_t* obj2 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj2 = 0;

    std::uint64_t* obj3 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj3 = 0x0;

    std::uint64_t* obj4 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj4 = 0xABBAEEABBA;

    desc.Deallocate(obj2);
    kassert(desc.NextFreeBuffer() == obj2);

    kout << "TryDeallocate1() Success!" << '\n';
  }

  inline void TryDeallocate2(){
    // TryDeallocate2()
    using BufferType = Prim::StaticArray<std::uint8_t, 64>;

    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 64, 2};
    desc.PoisonFreeArea(0x00);
    desc.SetupLinkage();

    BufferType* obj1 = reinterpret_cast<BufferType*>(desc.Allocate());
    Fill<std::uint8_t, 64>(*obj1, 0xAAu);

    BufferType* obj2 = reinterpret_cast<BufferType*>(desc.Allocate());
    Fill<std::uint8_t, 64>(*obj2, 0xBBu);

    BufferType* obj3 = reinterpret_cast<BufferType*>(desc.Allocate());
    Fill<std::uint8_t, 64>(*obj3, 0xCCu);

    BufferType* obj4 = reinterpret_cast<BufferType*>(desc.Allocate());
    Fill<std::uint8_t, 64>(*obj4, 0xDDu);

    desc.Deallocate(obj2);
    kassert(desc.NextFreeBuffer() == obj2);


    kout << "TryDeallocate2() Success!" << '\n';
  }

  inline void TryDoubleDeallocate1(){
    // TryDoubleDealloc1();
    kout << intmode::hex << "Base: " << BASE_ADDR << '\n';
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR), 8, 1};
    desc.PoisonFreeArea(0xBE);
    desc.SetupLinkage();

    std::uint64_t* obj1 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj1 = 0xDEADBEEF;

    std::uint64_t* obj2 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj2 = 0;

    std::uint64_t* obj3 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj3 = 0x0;

    std::uint64_t* obj4 = reinterpret_cast<std::uint64_t*>(desc.Allocate());
    *obj4 = 0xABBAEEABBA;

    desc.Deallocate(obj2);
    kassert(desc.NextFreeBuffer() == obj2);

    desc.Deallocate(obj2);
    kassert(desc.NextFreeBuffer() == obj2);

    kout << "TryDoubleDeallocate1() Success!" << '\n';
  }


}
