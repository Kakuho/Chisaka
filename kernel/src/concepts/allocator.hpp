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

  template<typename A>
  concept HeapAllocator = requires(A a, std::size_t nbytes, void* base){
    {a.AllocateObject(nbytes)} -> std::same_as<void*>;
    a.DeallocateObject(base);
  };

  template<typename A>
  concept Allocator = PageAllocator<A> || HeapAllocator<A>;
}
