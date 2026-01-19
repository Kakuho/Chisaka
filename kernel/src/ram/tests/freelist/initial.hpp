#pragma once

#include "./../../freelist.hpp"
#include "./../../../memory_map/memory_map_descriptor.hpp"
#include <cstdint>

namespace Mem::Page::Test::Freelist::Initial{

  void Init();

  void Clean();

  inline void Init(){
    MemoryMapDescriptor memmap{};
    Page::Freelist::Initialise(memmap);

    kassert(reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFree()) == 0x100000000);
    kassert(reinterpret_cast<std::uintptr_t>(Page::Freelist::NextFreeLow()) == 0x50000);
  }
}
