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

  void Discriminate_FullEntry1();    void Discriminate_FullEntry2();

  void MoveToFreeEmpty_FullHead();      void MoveToFreeEmpty_FullEntry();

  void MoveToFree_FullHead();     void MoveToFree_FullEntry();

  inline void Discriminate_FullEntry1(){
    // Premise:
    //  We have 4 full list descriptors
    //  We deallocate from one of them
    //  That one must move to the free list head
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), PAGE_SIZE, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), PAGE_SIZE, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), PAGE_SIZE, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), PAGE_SIZE, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();

    void* pobj = nullptr;

    desc1.Allocate();
    desc2.Allocate();
    pobj = desc3.Allocate();
    desc4.Allocate();

    kassert(desc3.ObjectInRange(pobj));

    Heap::Buffer buf{PAGE_SIZE};

    buf.AddList(&desc1);
    buf.AddList(&desc2);
    buf.AddList(&desc3);
    buf.AddList(&desc4);

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == nullptr);

    kassert(buf.FullListHead() == &desc1);
    kassert(buf.FullListHead()->NextList() == &desc2);
    kassert(buf.FullListHead()->NextList()->NextList() == &desc3);
    kassert(buf.FullListHead()->NextList()->NextList()->NextList() == &desc4);
    kassert(buf.FullListHead()->PrevList() == &desc4);

    //-------------------------------------------------------------

    kout << "Inifinite Loop?" << '\n';

    buf.Deallocate(pobj);

    kassert(buf.FreeListHead() == &desc3);
    kassert(buf.PartialListHead() == nullptr);

    kassert(buf.FullListHead() == &desc1);
    kassert(buf.FullListHead()->NextList() == &desc2);
    kassert(buf.FullListHead()->NextList()->NextList() == &desc4);
    kassert(buf.FullListHead()->PrevList() == &desc4);

    kout << "Discriminate_FullEntry() Success!" << '\n';
  }

  inline void Discriminate_FullEntry2(){
    // Premise:
    //  We have 1 free list descriptor and 3 full list descriptors
    //  We deallocate from one of the full list descriptors
    //  That one must move to the end of the free list head
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), PAGE_SIZE, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), PAGE_SIZE, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), PAGE_SIZE, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), PAGE_SIZE, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();

    void* pobj = nullptr;

    // desc1 is the free list, everything else is full
    desc2.Allocate();
    pobj = desc3.Allocate();
    desc4.Allocate();

    kassert(desc3.ObjectInRange(pobj));

    Heap::Buffer buf{PAGE_SIZE};

    buf.AddList(&desc1);
    buf.AddList(&desc2);
    buf.AddList(&desc3);
    buf.AddList(&desc4);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc1);
    kassert(buf.FreeListHead()->PrevList() == &desc1);

    kassert(buf.PartialListHead() == nullptr);

    kassert(buf.FullListHead() == &desc2);
    kassert(buf.FullListHead()->NextList() == &desc3);
    kassert(buf.FullListHead()->NextList()->NextList() == &desc4);
    kassert(buf.FullListHead()->PrevList() == &desc4);

    //-------------------------------------------------------------

    kout << "Inifinite Loop?" << '\n';

    buf.Deallocate(pobj);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc3);
    kassert(buf.FreeListHead()->PrevList() == &desc3);

    kassert(buf.PartialListHead() == nullptr);

    kassert(buf.FullListHead() == &desc2);
    kassert(buf.FullListHead()->NextList() == &desc4);
    kassert(buf.FullListHead()->PrevList() == &desc4);

    kout << "Discriminate_FullEntry2() Success!" << '\n';
  }

  inline void MoveToFreeEmpty_FullHead(){
    // Premise:
    // Deallocates correctly from the head of the full list descriptor
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR1), PAGE_SIZE, 1};
    desc.SetupLinkage();

    Heap::Buffer buf{PAGE_SIZE};

    buf.AddList(&desc);

    kassert(buf.FreeListHead() == &desc);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    void* pobj = buf.Allocate();

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == &desc);

    //-------------------------------------------------------------

    buf.Deallocate(pobj);

    kassert(buf.FreeListHead() == &desc);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    kout << "Discriminate_FullHead() Success!" << '\n';
  }

    inline void MoveToFreeEmpty_FullEntry(){
    // Premise:
    //
    //  We have 3 full list descriptors.
    //
    //  Given an empty free list, a deallocation from a free list causing it to
    //  become empty should move it to the free list header
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), PAGE_SIZE, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), PAGE_SIZE, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), PAGE_SIZE, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();

    void* pobj = nullptr;

    Heap::Buffer buf{PAGE_SIZE};

    desc1.Allocate();
    pobj = desc2.Allocate();
    desc3.Allocate();

    buf.AddList(&desc1);
    buf.AddList(&desc2);
    buf.AddList(&desc3);

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == nullptr);

    kassert(buf.FullListHead() == &desc1);
    kassert(buf.FullListHead()->NextList() == &desc2);
    kassert(buf.FullListHead()->NextList()->NextList() == &desc3);
    kassert(buf.FullListHead()->PrevList() == &desc3);

    //-------------------------------------------------------------

    buf.Deallocate(pobj);

    kassert(buf.PartialListHead() == nullptr);

    kassert(buf.FreeListHead() == &desc2);

    kassert(buf.FullListHead() == &desc1);
    kassert(buf.FullListHead()->NextList() == &desc3);
    kassert(buf.FullListHead()->PrevList() == &desc3);

    kout << "MoveToFree_FullEntry() Success!" << '\n';
  }

    inline void MoveToFree_FullHead(){
    // Premise:
    //  Given a free list, a deallocation from a full list causing it to
    //  become empty should move it to the free list header
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), PAGE_SIZE, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), PAGE_SIZE, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), PAGE_SIZE, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), PAGE_SIZE, 1};
    ListDescriptor desc5{reinterpret_cast<void*>(BASE_ADDR5), PAGE_SIZE, 1};
    ListDescriptor desc6{reinterpret_cast<void*>(BASE_ADDR6), PAGE_SIZE, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();
    desc5.SetupLinkage();
    desc6.SetupLinkage();

    // 4 5 6 are full descriptors

    void* pobj = nullptr;

    pobj = desc4.Allocate();
    desc5.Allocate();
    desc6.Allocate();

    Heap::Buffer buf{PAGE_SIZE};

    buf.AddList(&desc1);
    buf.AddList(&desc2);
    buf.AddList(&desc3);
    buf.AddList(&desc4);
    buf.AddList(&desc5);
    buf.AddList(&desc6);

    kassert(buf.PartialListHead() == nullptr);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc2);
    kassert(buf.FreeListHead()->NextList()->NextList() == &desc3);
    kassert(buf.FreeListHead()->PrevList() == &desc3);

    kassert(buf.FullListHead() == &desc4);
    kassert(buf.FullListHead()->NextList() == &desc5);
    kassert(buf.FullListHead()->NextList()->NextList() == &desc6);
    kassert(buf.FullListHead()->PrevList() == &desc6);

    //-------------------------------------------------------------

    buf.Deallocate(pobj);

    kassert(buf.PartialListHead() == nullptr);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc2);
    kassert(buf.FreeListHead()->NextList()->NextList() == &desc3);
    kassert(buf.FreeListHead()->NextList()->NextList()->NextList() == &desc4);
    kassert(buf.FreeListHead()->PrevList() == &desc4);

    kassert(buf.FullListHead() == &desc5);
    kassert(buf.FullListHead()->NextList() == &desc6);
    kassert(buf.FullListHead()->PrevList() == &desc6);

    kout << "MoveToFree_FullHead() Success!" << '\n';
  }

  inline void MoveToFree_FullEntry(){
    // Premise:
    //  Given a free list, a deallocation from a full list causing it to
    //  become empty should move it to the free list header
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), PAGE_SIZE, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), PAGE_SIZE, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), PAGE_SIZE, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), PAGE_SIZE, 1};
    ListDescriptor desc5{reinterpret_cast<void*>(BASE_ADDR5), PAGE_SIZE, 1};
    ListDescriptor desc6{reinterpret_cast<void*>(BASE_ADDR6), PAGE_SIZE, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();
    desc5.SetupLinkage();
    desc6.SetupLinkage();

    // 4 5 6 are full descriptors

    void* pobj = nullptr;

    desc4.Allocate();
    pobj = desc5.Allocate();
    desc6.Allocate();

    Heap::Buffer buf{PAGE_SIZE};

    buf.AddList(&desc1);
    buf.AddList(&desc2);
    buf.AddList(&desc3);
    buf.AddList(&desc4);
    buf.AddList(&desc5);
    buf.AddList(&desc6);

    kassert(buf.PartialListHead() == nullptr);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc2);
    kassert(buf.FreeListHead()->NextList()->NextList() == &desc3);
    kassert(buf.FreeListHead()->PrevList() == &desc3);

    kassert(buf.FullListHead() == &desc4);
    kassert(buf.FullListHead()->NextList() == &desc5);
    kassert(buf.FullListHead()->NextList()->NextList() == &desc6);
    kassert(buf.FullListHead()->PrevList() == &desc6);

    //-------------------------------------------------------------

    buf.Deallocate(pobj);

    kassert(buf.PartialListHead() == nullptr);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc2);
    kassert(buf.FreeListHead()->NextList()->NextList() == &desc3);
    kassert(buf.FreeListHead()->NextList()->NextList()->NextList() == &desc5);
    kassert(buf.FreeListHead()->PrevList() == &desc5);

    kassert(buf.FullListHead() == &desc4);
    kassert(buf.FullListHead()->NextList() == &desc6);
    kassert(buf.FullListHead()->PrevList() == &desc6);

    kout << "MoveToFree_FullEntry() Success!" << '\n';
  }

}
