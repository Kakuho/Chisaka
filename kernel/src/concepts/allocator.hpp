#pragma once

#include <cstddef>
#include <concepts>

namespace Chisaka::Concepts{
  template<typename A>
  concept PageAllocator = requires(A a, unsigned pages, void* base){
    {a.AllocatePage()} -> std::same_as<void*>;
    {a.AllocatePages(pages)} -> std::same_as<void*>;
    a.DeallocPage(base);
  };
}
