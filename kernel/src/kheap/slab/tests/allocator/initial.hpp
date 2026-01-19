#pragma once

#include <cstdint>

#include "drivers/serial/kostream.hpp"

#include "./../../allocator.hpp"
#include "./../../list_descriptor.hpp"
#include "./../../../page/page_allocator.hpp"

namespace Mem::Heap::Test::Allocator::Initial{

  static constexpr std::uintptr_t BASE_ADDR1 = 0x55000;
  static constexpr std::uintptr_t BASE_ADDR2 = 0x55000;
  static constexpr std::uintptr_t BASE_ADDR3 = 0x55000;
  static constexpr std::uintptr_t BASE_ADDR4 = 0x55000;
  static constexpr std::uintptr_t BASE_ADDR5 = 0x55000;

  static constexpr std::uint16_t PAGE_SIZE = 0x1000;

  void FixedAddr();
  void AddBuffer(); void AddList();

  inline void FixedAddr(){
    //  at the end of ctor, 
    //    the list descriptor should be in its own list
    PageAllocator::Initialise();
    Heap::Allocator::Initialise(reinterpret_cast<void*>(BASE_ADDR1));
    ListDescriptor* list = Heap::Allocator::Buffer64().PartialListHead();
    kassert(list);
    kassert(list == reinterpret_cast<ListDescriptor*>(BASE_ADDR2));
    kout << "FixedAddr Initalisation PASSED!" << '\n';
  }

  inline void AddBuffer(){
    //  Synopsis:
    //    Testing the basic debugging call
    Heap::Allocator::Initialise(reinterpret_cast<void*>(BASE_ADDR1));
  }

}
