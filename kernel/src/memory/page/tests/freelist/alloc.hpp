#pragma once

#include "./../../freelist.hpp"
#include "./../../../memory_map/memory_map_descriptor.hpp"
#include <cstdint>

namespace Mem::Page::Test::Freelist::Alloc{

  void Allocate();
  void AllocateZero();

  void AllocateLow();

  void AllocateMany1(); 
  void AllocateMany2_1();   
  void AllocateMany2_2(); 
  void AllocateMany3();

  void Exhaust1();
  void ExhaustLow1();

  inline void Allocate(){
    // Basic Single Allocation
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);

    std::uintptr_t baseofBase = 
      reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFree());

    [[maybe_unused]] void* unusedbase = Page::Freelist::AllocatePage();
    
    kout << "Next Free: " 
          << reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFree()) << '\n';

    kout << "Allocated: " 
          << reinterpret_cast<std::uintptr_t>(unusedbase) << '\n';

    kassert(reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFree()) 
        == baseofBase + 0x1000);

    kout << "Allocate() Success!" << '\n';
  }

  inline bool IsZeroPage(void* pageaddr){
    auto* pagebase = reinterpret_cast<Prim::StaticArray<std::uint8_t, 0x1000>*>(pageaddr);
    for(std::size_t i = 0; i < 0x1000; i++){
      if((*(pagebase))[i] != 0){
        return false;
      }
    }
    return true;
  }

  inline void AllocateZero(){
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);

    auto* page = reinterpret_cast<Prim::StaticArray<std::uint8_t, 0x1000>*>(Page::Freelist::NextFree());
    Fill<std::uint8_t, 0x1000>(*page, 0xAA);

    void* allocPage = Page::Freelist::AllocatePage(OPT::Z);
    kassert(IsZeroPage(allocPage));

    kout << "AllocateZero() Success!" << '\n';
  }

  inline void AllocateLow(){
    // Basic Single Allocation
    // Allocate from the lower address space
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);

    std::uintptr_t baseofBase = 
      reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFreeLow());

    [[maybe_unused]] void* unusedbase = Page::Freelist::AllocatePage(OPT::LOW);
    
    kout << "Next Free: " 
          << reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFreeLow()) << '\n';

    kassert(reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFreeLow()) 
        == baseofBase + 0x1000);

    kout << "AllocateLow() Success!" << '\n';
  }

  inline void AllocateMany1(){
    // Basic Easy Case
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);
    std::uintptr_t baseofBase = 
      reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFree());

    [[maybe_unused]] void* unusedbase = Page::Freelist::AllocatePages(4);
    
    kassert(reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFree()) 
        == baseofBase + 0x4000);


    kout << "AllocateMany1() Success!" << '\n';
  }

  inline void AllocateMany2_1(){
    // Case when we have to skip the current region for the next 
    // in order for contigous page
    //
    // Assumptions:
    //  Free Memory Map Regions:
    //   50000   - 9F000          0x4f pages
    //   100000  - 7FEA5000       0
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);
    
    void* leaky = nullptr;

    for(std::uint8_t i = 0; i < 0x4e; i++){
      leaky = Page::Freelist::AllocatePage(OPT::LOW);
    }

    kout << "Last Alloc: " << reinterpret_cast<std::uintptr_t>(leaky) << '\n'; 

    void* block = Page::Freelist::AllocatePages(10, OPT::LOW);
    
    kassert(0x100000 == reinterpret_cast<std::uintptr_t>(block));

    kout << "AllocateMany2_1() Success!" << '\n';
  }

  inline void AllocateMany2_2(){
    // Case when we have to skip the current region for the next 
    // in order for contigous page
    //
    // Assumptions:
    //  Free Memory Map Regions:
    //   50000    - 9F000          0x4f     pages
    //   100000   - ???           0x7fda5  pages
    //   7FF1F000                  2        pages
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);
    
    void* leaky = nullptr;

    for(auto i = 0; i < 0x4f + 0x7FDB5 - 1; i++){
      leaky = Page::Freelist::AllocatePage(OPT::LOW);
    }

    void* block = Page::Freelist::AllocatePages(2, OPT::LOW);

    kout << "Last Alloc: " << reinterpret_cast<std::uintptr_t>(leaky) << '\n'; 
    kout << "Block: " << reinterpret_cast<std::uintptr_t>(block) << '\n'; 
    
    kassert(0x7FF1F000 == reinterpret_cast<std::uintptr_t>(block));

    kout << "AllocateMany2_2() Success!" << '\n';
  }

  inline void AllocateMany3(){
    // Case when there is no pages left
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);
    
    for(void* current = Page::Freelist::AllocatePage(); 
        current != nullptr; 
        current = Page::Freelist::AllocatePage()){
      kassert(current > reinterpret_cast<void*>(0xFFFF'FFFF));
    }

    void* yay = Page::Freelist::AllocatePage();
    kassert(yay == nullptr);
    kout << "AllocateMany3() Success!" << '\n';
  }

  inline void AllocateManyLow(){
    // Implement Low 
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);
    std::uintptr_t baseofBase = 
      reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFreeLow());

    [[maybe_unused]] void* unusedbase = Page::Freelist::AllocatePages(4);
    
    kout << "Next Free: " 
          << reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFree()) << '\n';


    kout << "Allocated: " 
          << reinterpret_cast<std::uintptr_t>(unusedbase) << '\n';

    kassert(reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFree()) 
        == baseofBase + 0x4000 + 0x1000);

    kout << "AllocateMany1() Success!" << '\n';
  }

  inline void Exhaust1(){
    // Premise: 
    //  We exhaust the page allocator of its normal pages, 
    //  and we must never have a page below 0xFFFF'FFFF
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);
    
    for(void* current = Page::Freelist::AllocatePage(); 
        current != nullptr; 
        current = Page::Freelist::AllocatePage()){
      kassert(current > reinterpret_cast<void*>(0xFFFF'FFFF));
    }

    kout << "Exhaust1() Success!" << '\n';
  }

  inline void ExhaustLow1(){
    // Premise: 
    //  We exhaust the page allocator of its low pages, 
    //  we must never have a page above 0xFFFF'FFFF
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);
    
    for(void* current = Page::Freelist::AllocatePage(OPT::LOW); 
        current != nullptr; 
        current = Page::Freelist::AllocatePage(OPT::LOW)){
      kassert(current < reinterpret_cast<void*>(0xFFFF'FFFF));
    }

    kout << "ExhaustLow1() Success!" << '\n';
  }

}
