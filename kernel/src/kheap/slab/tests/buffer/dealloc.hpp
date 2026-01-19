#pragma once

#include <cstdint>

#include "./../../allocator.hpp"
#include "./../../list_descriptor.hpp"

//  The difficulty with deallocation is ensuring that:
//
//  i)    Discriminate:
//        Finds and calls the correct list descriptor deallocation routine
//          -> tests partial head
//          -> test partial list entry
//          -> tests full head
//          -> test full list entry
//
//  ii)   MoveFull:
//        Given a full list descriptor, and a deallocation from it, 
//        it should move the entry to the partial list
//          -> test when partial list is empty
//          -> test when partial list is non empty
//
//  iii)  MoveToFree:
//        Given a list descriptor in use, and a deallocation from it 
//        which causes it to become empty, it should move the entry to 
//        the free list
//          -> given free list is empty
//            -> test moves partial list head
//            -> test moves partial list entry
//            -> test moves full list head
//            -> test moves full list entry
//          -> given free list is not empty
//            -> test moves partial list head
//            -> test moves partial list entry
//            -> test moves full list head
//            -> test moves full list entry
//
//  iv)   Thrash:
//          -> Exhaust a list descriptor, clean it, exhaust it etc.

namespace Mem::Heap::Test::Buffer::Dealloc{

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

  void Discriminate_PartialHead();  
  void Discriminate_PartialEntry1();  void Discriminate_PartialEntry2();  
  void Discriminate_FullHead();       
  void Discriminate_FullEntry1();    void Discriminate_FullEntry2();

  void MoveToFreeEmpty_PartialHead();   void MoveToFreeEmpty_PartialEntry();
  void MoveToFreeEmpty_FullHead();      void MoveToFreeEmpty_FullEntry();     

  void MoveToFree_PartialHead();  void MoveToFree_PartialEntry();
  void MoveToFree_FullHead();     void MoveToFree_FullEntry();

  void Thrash1(); void Thrash2();

  //-------------------------------------------------------------

  inline void Discriminate_PartialHead(){
    // Premise:
    //  Should call deallocation on the partial head
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR1), 10, 1};
    desc.SetupLinkage();

    Heap::Buffer buf{10};
    buf.AddList(&desc);

    kassert(buf.FreeListHead() == &desc);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    void* pobj = buf.Allocate();

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == &desc);
    kassert(buf.FullListHead() == nullptr);

    buf.Deallocate(pobj);

    kassert(buf.FreeListHead() == &desc);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    kout << "Discriminate_PartialHead() Success!" << '\n';
  }

  inline void Discriminate_PartialEntry1(){
    // Premise:
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 10, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 10, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();

    desc1.Allocate();
    void* pobj = desc2.Allocate();

    Heap::Buffer buf{10};

    buf.AddList(&desc1);
    buf.AddList(&desc2);

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc2);
    kassert(buf.PartialListHead()->PrevList() == &desc2);

    //-------------------------------------------------------------

    kout << "Inifnite Loop?" << '\n';

    buf.Deallocate(pobj);

    kout << "Outside of deallocate" << '\n';

    kassert(buf.FreeListHead() == &desc2);
    kassert(buf.FullListHead() == nullptr);

    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc1 );
    kassert(buf.PartialListHead()->PrevList() == &desc1);

    kout << "Discriminate_PartialEntry1() Success!" << '\n';
  }

  inline void Discriminate_PartialEntry2(){
    // Premise:
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 10, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 10, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), 10, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), 10, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();

    desc1.Allocate();
    desc2.Allocate();
    void* pobj = desc3.Allocate();
    desc4.Allocate();

    kassert(desc3.ObjectInRange(pobj));

    Heap::Buffer buf{10};

    buf.AddList(&desc1);
    buf.AddList(&desc2);
    buf.AddList(&desc3);
    buf.AddList(&desc4);

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc2);
    kassert(buf.PartialListHead()->NextList()->NextList() == &desc3);
    kassert(buf.PartialListHead()->NextList()->NextList()->NextList() == &desc4);
    kassert(buf.PartialListHead()->PrevList() == &desc4);

    //-------------------------------------------------------------

    kout << "Inifnite Loop?" << '\n';

    buf.Deallocate(pobj);

    kout << "Outside of deallocate" << '\n';

    kassert(buf.FreeListHead() == &desc3);
    kassert(buf.FullListHead() == nullptr);

    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc2);
    kassert(buf.PartialListHead()->NextList()->NextList() == &desc4);
    kassert(buf.PartialListHead()->PrevList() == &desc4);

    kout << "Discriminate_PartialEntry1() Success!" << '\n';
  }

  inline void Discriminate_FullHead(){
    // Premise:
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR1), 10, 1};
    desc.SetupLinkage();

    Heap::Buffer buf{10};

    buf.AddList(&desc);

    kassert(buf.FreeListHead() == &desc);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    void* pobj = nullptr;

    for(auto i = 0; i < desc.TotalBuffers(); i++){
      pobj = buf.Allocate();
    }

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == &desc);

    //-------------------------------------------------------------

    buf.Deallocate(pobj);

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == &desc);
    kassert(buf.FullListHead() == nullptr);

    kout << "Discriminate_FullHead() Success!" << '\n';
  }

  inline void Discriminate_FullEntry1(){
    // Premise:
    //  We have 4 full list descriptors
    //  We deallocate from one of them
    //  That one must move to the partial list head
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 10, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 10, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), 10, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), 10, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();

    void* pobj = nullptr;

    for(auto i = 0; i < desc1.TotalBuffers(); i++){
      desc1.Allocate();
    }
    for(auto i = 0; i < desc2.TotalBuffers(); i++){
      desc2.Allocate();
    }
    for(auto i = 0; i < desc3.TotalBuffers(); i++){
      pobj = desc3.Allocate();
    }
    for(auto i = 0; i < desc4.TotalBuffers(); i++){
      desc4.Allocate();
    }

    kassert(desc3.ObjectInRange(pobj));

    Heap::Buffer buf{10};

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

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == &desc3);

    kassert(buf.FullListHead() == &desc1);
    kassert(buf.FullListHead()->NextList() == &desc2);
    kassert(buf.FullListHead()->NextList()->NextList() == &desc4);
    kassert(buf.FullListHead()->PrevList() == &desc4);

    kout << "Discriminate_FullEntry() Success!" << '\n';
  }

  inline void Discriminate_FullEntry2(){
    // Premise:
    //  We have 1 partial list descriptor and 3 full list descriptors
    //  We deallocate from one of the full list descriptors
    //  That one must move to the end of the partial list head
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 10, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 10, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), 10, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), 10, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();

    void* pobj = nullptr;

    // desc1 is partial
    desc1.Allocate();

    for(auto i = 0; i < desc2.TotalBuffers(); i++){
      desc2.Allocate();
    }
    for(auto i = 0; i < desc3.TotalBuffers(); i++){
      pobj = desc3.Allocate();
    }
    for(auto i = 0; i < desc4.TotalBuffers(); i++){
      desc4.Allocate();
    }

    kassert(desc3.ObjectInRange(pobj));

    Heap::Buffer buf{10};

    buf.AddList(&desc1);
    buf.AddList(&desc2);
    buf.AddList(&desc3);
    buf.AddList(&desc4);

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc1);
    kassert(buf.PartialListHead()->PrevList() == &desc1);

    kassert(buf.FullListHead() == &desc2);
    kassert(buf.FullListHead()->NextList() == &desc3);
    kassert(buf.FullListHead()->NextList()->NextList() == &desc4);
    kassert(buf.FullListHead()->PrevList() == &desc4);

    //-------------------------------------------------------------

    kout << "Inifinite Loop?" << '\n';

    buf.Deallocate(pobj);

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == &desc1);
    kassert(buf.PartialListHead()->NextList() == &desc3);
    kassert(buf.PartialListHead()->PrevList() == &desc3);

    kassert(buf.FullListHead() == &desc2);
    kassert(buf.FullListHead()->NextList() == &desc4);
    kassert(buf.FullListHead()->PrevList() == &desc4);

    kout << "Discriminate_FullEntry2() Success!" << '\n';
  }

  inline void MoveToFreeEmpty_PartialHead(){
    // Premise:
    //  Given an empty a free list, a deallocation from a partial list 
    //  should move it to the free list header
    Discriminate_PartialHead();
  }

  inline void MoveToFreeEmpty_PartialEntry(){
    // Premise:
    //  Given an empty a free list, and a non empty partial list, a deallocation 
    //  from a partial list causing it to become empty should move it to the free 
    //  list header
    Discriminate_PartialEntry2();
  }

  inline void MoveToFreeEmpty_FullHead(){
    // Premise:
    //  Given an empty a free list, a deallocation from a free list causing it to
    //  become empty should move it to the free list header
    ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR1), 8, 1};
    desc.SetupLinkage();

    Heap::Buffer buf{8};

    buf.AddList(&desc);

    kassert(buf.FreeListHead() == &desc);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    void* pobj = nullptr;

    for(auto i = 0; i < desc.TotalBuffers(); i++){
      pobj = buf.Allocate();
    }

    kassert(buf.FreeListHead() == nullptr);
    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == &desc);

    //-------------------------------------------------------------

    for(std::uintptr_t curr = desc.Base(); curr < desc.Limit(); curr += 8){
      buf.Deallocate(reinterpret_cast<void*>(curr));
    }

    kassert(buf.FullListHead() == nullptr);
    kassert(buf.FreeListHead() == &desc);
    kassert(buf.PartialListHead() == nullptr);

    kout << "MoveToFree_FullHead() Success!" << '\n';
  }

  inline void MoveToFreeEmpty_FullEntry(){
    // Premise:
    //
    //  We have 3 full list descriptors.
    //
    //  Given an empty free list, a deallocation from a free list causing it to
    //  become empty should move it to the free list header
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 8, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 8, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), 8, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();

    void* pobj = nullptr;

    Heap::Buffer buf{8};

    for(auto i = 0; i < desc1.TotalBuffers(); i++){
      pobj = desc1.Allocate();
    }
    for(auto i = 0; i < desc2.TotalBuffers(); i++){
      pobj = desc2.Allocate();
    }
    for(auto i = 0; i < desc3.TotalBuffers(); i++){
      pobj = desc3.Allocate();
    }

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

    for(std::uintptr_t curr = desc2.Base(); curr < desc2.Limit(); curr += 8){
      buf.Deallocate(reinterpret_cast<void*>(curr));
    }

    kassert(buf.PartialListHead() == nullptr);

    kassert(buf.FreeListHead() == &desc2);

    kassert(buf.FullListHead() == &desc1);
    kassert(buf.FullListHead()->NextList() == &desc3);
    kassert(buf.FullListHead()->PrevList() == &desc3);

    kout << "MoveToFree_FullEntry() Success!" << '\n';
  }


  inline void MoveToFree_PartialHead(){
    // Premise:
    //  Given an nonempty a free list, a deallocation from the partial list head
    //  should move it to the free list header
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 10, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR1), 10, 1};
    desc1.SetupLinkage();
    desc2.SetupLinkage();

    void* pobj = desc2.Allocate();

    Heap::Buffer buf{10};

    buf.AddList(&desc1);
    buf.AddList(&desc2);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc1);
    kassert(buf.FreeListHead()->PrevList() == &desc1);

    kassert(buf.PartialListHead() == &desc2);
    kassert(buf.PartialListHead()->NextList() == &desc2);
    kassert(buf.PartialListHead()->PrevList() == &desc2);

    kassert(buf.FullListHead() == nullptr);

    //-------------------------------------------------------------
    
    buf.Deallocate(pobj);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc2);
    kassert(buf.FreeListHead()->PrevList() == &desc2);

    kassert(buf.PartialListHead() == nullptr);
    kassert(buf.FullListHead() == nullptr);

    kout << "MoveToFree_PartialHead() Success!" << '\n';
  }

  inline void MoveToFree_PartialEntry(){
    // Premise:
    //  Given an nonempty a free list, a deallocation from a partial list 
    //  should move it to the free list header
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 10, 1};   // free head
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 10, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), 10, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), 10, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();

    desc2.Allocate();
    void* pobj = desc3.Allocate();
    desc4.Allocate();

    Heap::Buffer buf{10};

    buf.AddList(&desc1);
    buf.AddList(&desc2);
    buf.AddList(&desc3);
    buf.AddList(&desc4);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc1);
    kassert(buf.FreeListHead()->PrevList() == &desc1);

    kassert(buf.PartialListHead() == &desc2);
    kassert(buf.PartialListHead()->NextList() == &desc3);
    kassert(buf.PartialListHead()->NextList()->NextList() == &desc4);
    kassert(buf.PartialListHead()->PrevList() == &desc4);

    kassert(buf.FullListHead() == nullptr);

    //-------------------------------------------------------------
    
    buf.Deallocate(pobj);

    kassert(buf.FreeListHead() == &desc1);
    kassert(buf.FreeListHead()->NextList() == &desc3);
    kassert(buf.FreeListHead()->PrevList() == &desc3);

    kassert(buf.PartialListHead() == &desc2);
    kassert(buf.PartialListHead()->NextList() == &desc4);
    kassert(buf.PartialListHead()->PrevList() == &desc4);

    kassert(buf.FullListHead() == nullptr);

    kout << "MoveToFree_PartialEntry() Success!" << '\n';
  }

  inline void MoveToFree_FullHead(){
    // Premise:
    //  Given a free list, a deallocation from a full list causing it to
    //  become empty should move it to the free list header
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 8, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 8, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), 8, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), 8, 1};
    ListDescriptor desc5{reinterpret_cast<void*>(BASE_ADDR5), 8, 1};
    ListDescriptor desc6{reinterpret_cast<void*>(BASE_ADDR6), 8, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();
    desc5.SetupLinkage();
    desc6.SetupLinkage();

    // 4 5 6 are full descriptors

    void* pobj = nullptr;

    for(auto i = 0; i < desc4.TotalBuffers(); i++){
      desc4.Allocate();
    }
    for(auto i = 0; i < desc5.TotalBuffers(); i++){
      pobj = desc5.Allocate();
    }
    for(auto i = 0; i < desc6.TotalBuffers(); i++){
      desc6.Allocate();
    }

    Heap::Buffer buf{8};

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

    for(std::uintptr_t curr = desc4.Base(); curr < desc4.Limit(); curr += 8){
      buf.Deallocate(reinterpret_cast<void*>(curr));
    }

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

    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 8, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 8, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), 8, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), 8, 1};
    ListDescriptor desc5{reinterpret_cast<void*>(BASE_ADDR5), 8, 1};

    ListDescriptor desc6{reinterpret_cast<void*>(BASE_ADDR6), 8, 1};

    desc1.SetupLinkage();
    desc2.SetupLinkage();
    desc3.SetupLinkage();
    desc4.SetupLinkage();
    desc5.SetupLinkage();
    desc6.SetupLinkage();

    // 4 5 6 are full descriptors

    void* pobj = nullptr;

    for(auto i = 0; i < desc4.TotalBuffers(); i++){
      desc4.Allocate();
    }
    for(auto i = 0; i < desc5.TotalBuffers(); i++){
      pobj = desc5.Allocate();
    }
    for(auto i = 0; i < desc6.TotalBuffers(); i++){
      desc6.Allocate();
    }

    Heap::Buffer buf{8};

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

    for(std::uintptr_t curr = desc5.Base(); curr < desc5.Limit(); curr += 8){
      buf.Deallocate(reinterpret_cast<void*>(curr));
    }

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

  inline void Thrash1(){
    // Premise:
    //    We initially have 3 free list descriptors, 2 full descriptors
    //    We empty the full descriptor and then it exhaust it, observing that
    //    it ends up in the correct lists.
    ListDescriptor desc1{reinterpret_cast<void*>(BASE_ADDR1), 8, 1};
    ListDescriptor desc2{reinterpret_cast<void*>(BASE_ADDR2), 8, 1};
    ListDescriptor desc3{reinterpret_cast<void*>(BASE_ADDR3), 8, 1};
    ListDescriptor desc4{reinterpret_cast<void*>(BASE_ADDR4), 8, 1};
    ListDescriptor desc5{reinterpret_cast<void*>(BASE_ADDR5), 8, 1};

  }


}
