#pragma once

#include <cstdint>

#include "drivers/serial/kostream.hpp"


#include "./../../allocator.hpp"
#include "./../../buffer.hpp"
#include "./../../list_descriptor.hpp"
#include "memory/page/page_allocator.hpp"

namespace Mem::Heap::Test::Allocator::Alloc{

  static constexpr std::uintptr_t BASE_ADDR1  = 0x55000;
  static constexpr std::uintptr_t BASE_ADDR2  = 0x56000;
  static constexpr std::uintptr_t BASE_ADDR3  = 0x57000;
  static constexpr std::uintptr_t BASE_ADDR4  = 0x58000;
  static constexpr std::uintptr_t BASE_ADDR5  = 0x59000;
  static constexpr std::uintptr_t BASE_ADDR6  = 0x60000;
  static constexpr std::uintptr_t BASE_ADDR7  = 0x61000;
  static constexpr std::uintptr_t BASE_ADDR8  = 0x62000;
  static constexpr std::uintptr_t BASE_ADDR9  = 0x63000;
  static constexpr std::uintptr_t BASE_ADDR10 = 0x64000;

  static constexpr std::uint16_t PAGE_SIZE = 0x1000;


  void AllocateDebug();
  void Allocate();

  inline void AllocateDebug(){
    PageAllocator::Initialise();
    Heap::ListDescriptor desc{reinterpret_cast<void*>(BASE_ADDR2), 8, 1};
    Heap::Allocator::Initialise(reinterpret_cast<void*>(BASE_ADDR1));
  }

  inline void Allocate400(){
    PageAllocator::Initialise();
    Heap::Allocator::Initialise();

    Heap::ListDescriptor ld400{reinterpret_cast<void*>(BASE_ADDR1), 0x400, 1};
    kassert(ld400.BufferSize() == 0x400);

    Heap::Buffer buff400{0x400};
    kassert(buff400.BufferSize() == 0x400);

    buff400.AddList(&ld400);

    Heap::Allocator::AddBuffer(&buff400);

    kassert(Heap::Allocator::Buffer64().NextBuffer() == &buff400);
    kassert(Heap::Allocator::Buffer64().PrevBuffer() == &buff400);
    kassert(Heap::Allocator::BufferExists(0x400));

    void* guy = Heap::Allocator::Allocate(0x400);
    
    kout << "allocate400 is fin" << '\n';
  }
  
  inline void Allocate(){
    PageAllocator::Initialise();
    Heap::Allocator::Initialise();

    void* guy1 = Heap::Allocator::Allocate(0x400);
    void* guy2 = Heap::Allocator::Allocate(0x400);
    void* guy3 = Heap::Allocator::Allocate(0x400);
    void* guy4 = Heap::Allocator::Allocate(0x400);
    void* guy5 = Heap::Allocator::Allocate(0x400);
    void* guy6 = Heap::Allocator::Allocate(0x400);
    void* guy7 = Heap::Allocator::Allocate(0x400);
    void* guy8 = Heap::Allocator::Allocate(0x400);

    kout << "guy1: " << reinterpret_cast<std::uintptr_t>(guy1) << '\n';
    kout << "guy2: " << reinterpret_cast<std::uintptr_t>(guy2) << '\n';
    kout << "guy3: " << reinterpret_cast<std::uintptr_t>(guy3) << '\n';
    kout << "guy4: " << reinterpret_cast<std::uintptr_t>(guy4) << '\n';
    kout << "guy5: " << reinterpret_cast<std::uintptr_t>(guy5) << '\n';
    kout << "guy6: " << reinterpret_cast<std::uintptr_t>(guy6) << '\n';
    kout << "guy7: " << reinterpret_cast<std::uintptr_t>(guy7) << '\n';
    kout << "guy8: " << reinterpret_cast<std::uintptr_t>(guy8) << '\n';

    kout << "End of Allocate()" << '\n';
  }

  inline void AllocateCLBFis(){
    PageAllocator::Initialise();
    Heap::Allocator::Initialise();

    Heap::Buffer& bob = Heap::Allocator::Buffer64();

    kassert(bob.NextBuffer() == &bob);
    kassert(bob.PrevBuffer() == &bob);

    for(std::uint8_t i = 0; i < 32; i++){
      void* leaky = Heap::Allocator::Allocate(0x400);
      kout << "CLB " << i << ": " << reinterpret_cast<std::uintptr_t>(leaky) << '\n';
    }

    kassert(bob.NextBuffer()->BufferSize() == 0x400);
    kassert(bob.PrevBuffer()->BufferSize() == 0x400);

    for(std::uint8_t i = 0; i < 32; i++){
      void* leaky = Heap::Allocator::Allocate(0x100);
      kout << "CLB " << i << ": " << reinterpret_cast<std::uintptr_t>(leaky) << '\n';
    }

    kassert(bob.NextBuffer()->BufferSize() == 0x400);
    kassert(bob.NextBuffer()->NextBuffer()->BufferSize() == 0x100);
    kassert(bob.PrevBuffer()->BufferSize() == 0x100);
    kout << "AllocateCLBFis() Success!" << '\n';
  }
}
