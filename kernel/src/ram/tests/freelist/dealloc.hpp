#pragma once

#include "./../../freelist.hpp"
#include "./../../../memory_map/memory_map_descriptor.hpp"
#include <cstdint>

namespace Mem::Page::Test::Freelist::Dealloc{
  void Deallocate();

  void DeallocateFromMany();  // Requires Page Map

  void Refill1();               // Requires Page Map

  inline void Deallocate(){
    // Simply tests a singular deallocation
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);

    void* page1 = Page::Freelist::AllocatePage();
    void* page2 = Page::Freelist::AllocatePage();
    void* page3 = Page::Freelist::AllocatePage();

    Page::Freelist::FreePage(page2);
    kassert(Page::Freelist::NextFree() == page2);
    kout << "Deallocate() Success!" << '\n';
  }

  inline void DeallocateFromMany(){
    // Premise:
    //  Given we have allocated a block of pages and a deallocation from those pages,
    //  we should be able to allocate the pages within that block individually
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);

    void* page1 = Page::Freelist::AllocatePage();
    void* pageBlock = Page::Freelist::AllocatePages(3);
    void* page3 = Page::Freelist::AllocatePage();

    Page::Freelist::FreePage(pageBlock);

    //-------------------------------------------------------------

    kout << "pageblock: " << reinterpret_cast<std::uintptr_t>(pageBlock) << '\n';

    void* pb1 = Page::Freelist::AllocatePage();
    kout << "pb1: " << reinterpret_cast<std::uintptr_t>(pb1) << '\n';

    kassert(pb1 == pageBlock);

    void* pb2 = Page::Freelist::AllocatePage();
    kout << "pb2: " << reinterpret_cast<std::uintptr_t>(pb2) << '\n';
    kassert(reinterpret_cast<std::uintptr_t>(pb2) == reinterpret_cast<std::uintptr_t>(pageBlock) + 0x1000);

    void* pb3 = Page::Freelist::AllocatePage();
    kassert(reinterpret_cast<std::uintptr_t>(pb3) == reinterpret_cast<std::uintptr_t>(pageBlock) + 0x2000);
    kassert(reinterpret_cast<std::uintptr_t>(pb3) + 0x1000 == reinterpret_cast<std::uintptr_t>(page3));

    kout << "DeallocateMany() Success!" << '\n';
  }

  inline void Refill1(){
    //  Premise:
    //    Exhaust the page allocator, then start refilling it
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);

    void* base = Page::Freelist::AllocatePage();
    void* leaky = base;

    while(leaky){
      leaky = Page::Freelist::AllocatePage();
    }

    kout << "Finish Exhausting" << '\n';

    for(auto start = reinterpret_cast<std::uintptr_t>(base); 
        start <= 0x280000000; 
        start += 0x1000){
      Page::Freelist::FreePage(reinterpret_cast<void*>(start));
      kout << start << " returned to allocator\n";
    }

    kout << "Finish Refilling" << '\n';

    kassert(Page::Freelist::NextFree() == base);
  }

  inline void Refill2(){
    //  Premise:
    //    Exhaust the page allocator, then start refilling it
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);

    void* base = Page::Freelist::AllocatePage();
    void* leaky = base;

    while(leaky){
      leaky = Page::Freelist::AllocatePage();
    }

    kout << "Finish Exhausting" << '\n';

    for(auto start = 0x280000000ul; 
        start >= reinterpret_cast<std::uintptr_t>(base); 
        start -= 0x1000){
      Page::Freelist::FreePage(reinterpret_cast<void*>(start));
      kout << start << " returned to allocator\n";
    }

    kout << "Finish Refilling" << '\n';

    kassert(Page::Freelist::NextFree() == base);
  }
}
