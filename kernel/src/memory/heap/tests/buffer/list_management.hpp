#pragma once

#include <cstdint>

#include "./../../allocator.hpp"
#include "./../../list_descriptor.hpp"

namespace Mem::Heap::Test::Buffer::ListMgt{

  static constexpr std::uintptr_t BASE_ADDR1 = 0x55000;
  static constexpr std::uintptr_t BASE_ADDR2 = 0x56000;
  static constexpr std::uintptr_t BASE_ADDR3 = 0x57000;
  static constexpr std::uintptr_t BASE_ADDR4 = 0x58000;
  static constexpr std::uintptr_t BASE_ADDR5 = 0x59000;
  static constexpr std::uintptr_t BASE_ADDR6 = 0x60000;
  static constexpr std::uintptr_t BASE_ADDR7 = 0x61000;
  static constexpr std::uintptr_t BASE_ADDR8 = 0x62000;

  static constexpr std::uintptr_t VBASE_ADDR = 0xFFFF800000055000;
  static constexpr std::uint16_t PAGE_SIZE = 0x1000;

  void TryMetadata();

  void TryAddLD(); void TryAddLD2(); void TryAddLD3(); 

  inline void TryMetadata1(){
    Heap::Buffer buf{10};
    kassert(buf.BufferSize() == 10);
    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);
  }

  inline void TryAddLD(){
    // TryAddLD()
    // Premise:
    //  Adding List Descriptors to the buffers
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR1), 10, 1};
    Heap::Buffer buf{10};
    buf.AddList(&desc);
    kassert(buf.BufferSize() == 10);
    kassert(buf.FreeListHead() == &desc);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);
  }

  inline void TryAddLD2(){
    // TryAddLD2()
    // Premise:
    //  Ensures list descriptors gets added to the correct list of the buffer
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 64, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 64, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), 64, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), 64, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();

    // d1 d3 shall be partial, d2 d4 shall be free
    desc1.Allocate();
    desc3.Allocate();

    Heap::Buffer buf{64};

    buf.AddList(&desc1);
    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == &desc1);

    buf.AddList(&desc2);
    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.PartialListHead() == &desc1);

    buf.AddList(&desc3);
    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc3);


    buf.AddList(&desc4);
    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.FreeListHead()->NextList() == &desc4);
    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc3);

    kout << "TryAddLD2 Success" << '\n';
  }
  
  inline void TryAddLD3(){
    // TryAddLD2()
    // Premise:
    //  Ensures list descriptors gets added to the correct list of the buffer
    //  Has full list descriptors, partial list descriptors and free list descriptors
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 64, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 64, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), 64, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), 64, 1};
    ListDescriptor desc5{reinterpret_cast<void*>(BASE_ADDR3), 64, 1};
    ListDescriptor desc6{reinterpret_cast<void*>(BASE_ADDR4), 64, 1};

    Heap::Buffer buf{64};

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

    buf.AddList(&desc1);
    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.FullListHead() == nullptr);

    buf.AddList(&desc2);
    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.FullListHead() == nullptr);

    buf.AddList(&desc3);
    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc3);
    kassert(buf.FullListHead() == nullptr);

    buf.AddList(&desc4);
    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.FreeListHead()->NextList() == &desc4);
    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc3);
    kassert(buf.FullListHead() == nullptr);

    buf.AddList(&desc5);
    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.FreeListHead()->NextList() == &desc4 &&
          buf.FreeListHead()->PrevList() == &desc4);

    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc3 &&
          buf.PartialListHead()->PrevList() == &desc3);

    kassert(buf.FullListHead() == &desc5);
    kassert(buf.FullListHead()->NextList() == &desc5 &&
          buf.FullListHead()->PrevList() == &desc5);

    buf.AddList(&desc6);
    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.FreeListHead()->NextList() == &desc4);

    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc3);

    kassert(buf.FullListHead() == &desc5);
    kassert(buf.FullListHead()->NextList() == &desc6);
    kassert(buf.FullListHead()->PrevList() == &desc6);

    kout << "TryAddLD3 Success" << '\n';
  }
}
