#pragma once

#include <cstdint>

#include "./../../allocator.hpp"
#include "./../../list_descriptor.hpp"

// The main problem of allocation is seeing that the free list
// gets moved to the partial list, and that the partial list gets moved 
// to the full list.

namespace Mem::Heap::Test::Buffer::Alloc{

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

  void TryAllocate_Mock1(); void TryAllocate_Mock2();

  void TryExhaust1(); 
  void TryExhaust2();
  void TryExhaust3();

  inline void TryAllocate_Mock1(){
    // TryAllocate_Mock1()
    // Simple check to see if the initial free list gets moved to the partial list
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR1), 10, 1};
    desc.SetupLinkage();

    Heap::Buffer buf{10};
    buf.AddList(&desc);

    kassert(buf.FreeListHead() == &desc);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    void* nextfree = desc.NextFreeBuffer();
    void* allocd = buf.Allocate();

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == &desc);
    kassert(buf.FullListHead() == nullptr);

    kassert(allocd == nextfree);

    void* nextfree2 = desc.NextFreeBuffer();
    void* allocd2 = buf.Allocate();

    kassert(allocd2 == nextfree2);
    kassert(allocd2 != allocd);

    kout << "TryAllocate_MOCK1 Success" << '\n';
  }

  inline void TryAllocate_Mock2(){
    // TryAllocate_Mock2()
    // Premise:
    //  Have a partial list, and a free list, make sure 
    //  that the next allocated belongs to the partial list descriptor
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

    // this needs to come afterwards to ensure correct calculations
    buf.AddList(&desc1);
    buf.AddList(&desc2);
    buf.AddList(&desc3);
    buf.AddList(&desc4);

    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.FreeListHead()->NextList() == &desc4);
    kassert(buf.FreeListHead()->PrevList() == &desc4);

    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc3);
    kassert(buf.PartialListHead()->PrevList() == &desc3);

    kassert(buf.FullListHead() == nullptr);

    // the next allocated should be from desc1
    void* nextfree = desc1.NextFreeBuffer();
    void* allocd = buf.Allocate();

    kassert(allocd == nextfree);

    // Ensure correct list linkages
    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.FreeListHead()->NextList() == &desc4);
    kassert(buf.FreeListHead()->PrevList() == &desc4);

    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc3);
    kassert(buf.PartialListHead()->PrevList() == &desc3);

    kassert(buf.FullListHead() == nullptr);
    kout << "TryAllocate_Mock2 Success!" << '\n';
  }

  inline void TryExhaust1(){
    // TryExhaust1()
    // Premise:
    //  We have a free list
    //  We exhaust it, and it should move to the full list
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 64, 1};
    desc1.SetupLinkage();

    Heap::Buffer buf{64};
    buf.AddList(&desc1);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc1);
    kassert(buf.FreeListHead()->PrevList() == &desc1);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    for(auto i = 0; i < desc1.TotalBuffers(); i++){
      // Allocate from the buffer - overshoots a bit
      buf.Allocate();
    }

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == &desc1);
    kassert(buf.FullListHead()->NextList() == &desc1);
    kassert(buf.FullListHead()->PrevList() == &desc1);

    kout << "TryExhaust1 Success!" << '\n';
  }

  inline void TryExhaust2(){
    // Premise:
    //  Initially we have 2 free lists.
    //  We exhaust one.
    //  The next allocation should come from the next free list present
    //  Tests that the lists move correctly in 2 lists
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 64, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 64, 1};
    desc1.SetupLinkage();
    desc2.SetupLinkage();

    Heap::Buffer buf{64};

    buf.AddList(&desc1);
    buf.AddList(&desc2);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc2);
    kassert(buf.FreeListHead()->PrevList() == &desc2);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    //-------------------------------------------------------------

    for(auto i = 0; i < desc1.TotalBuffers(); i++){
      buf.Allocate();
    }

    kassert(buf.FullListHead() == &desc1);
    kassert(buf.FullListHead()->NextList() == &desc1);
    kassert(buf.FullListHead()->PrevList() == &desc1);

    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.FreeListHead()->NextList() == &desc2);
    kassert(buf.FreeListHead()->PrevList() == &desc2);

    void* nextfree = desc2.NextFreeBuffer();
    void* allocd = buf.Allocate();

    kassert(allocd == nextfree);
    kout << "TryExhaust2 Success!" << '\n';
  }

  inline void TryExhaust3(){
    // Premise:
    //  Initially we have 5 lists.
    //  2 Free, 2 Partial, 1 Full
    //
    //  We exhaust one of the partial lists.
    //  The next allocation should come from the next partial list present
    //
    //  Tests that the lists move correctly
    //  End:
    //  2 free, 1 partial, 1 full
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 64, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 64, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), 64, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), 64, 1};
    ListDescriptor desc5{reinterpret_cast<void*>(BASE_ADDR5), 64, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();
    desc5.SetupLinkage();

    //-------------------------------------------------------------
    
    // desc1, desc2 are partial, 
    // desc3 is full, 
    // desc4, desc5 are free.

    desc1.Allocate();
    desc2.Allocate();
    for(auto i = 0; i < desc3.TotalBuffers(); i++){
      desc3.Allocate();
    }

    //-------------------------------------------------------------

    Heap::Buffer buf{64};

    buf.AddList(&desc1);
    buf.AddList(&desc2);
    buf.AddList(&desc3);
    buf.AddList(&desc4);
    buf.AddList(&desc5);

    kassert(buf.FreeListHead() == &desc4);
    kassert(buf.FreeListHead()->NextList() == &desc5);
    kassert(buf.FreeListHead()->PrevList() == &desc5);

    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc2);
    kassert(buf.PartialListHead()->PrevList() == &desc2);

    kassert(buf.FullListHead() == &desc3);

    //-------------------------------------------------------------

    for(auto i = 0; i < desc1.TotalBuffers() - 1; i++){
      buf.Allocate();
    }

    // buffer should allocate from the partial list first, so 
    // free list is not touched
    kassert(buf.FreeListHead() == &desc4);
    kassert(buf.FreeListHead()->NextList() == &desc5);
    kassert(buf.FreeListHead()->PrevList() == &desc5);

    kassert(buf.PartialListHead() == &desc2);
    kassert(buf.PartialListHead()->NextList() == &desc2);
    kassert(buf.PartialListHead()->PrevList() == &desc2);

    kassert(buf.FullListHead() == &desc3);
    kassert(buf.FullListHead()->NextList() == &desc1);
    kassert(buf.FullListHead()->PrevList() == &desc1);

    void* nextfree = desc2.NextFreeBuffer();
    void* allocd = buf.Allocate();

    kassert(allocd == nextfree);
    kout << "TryExhaust3 Success!" << '\n';
  }

}
