#pragma once

#include <cstddef>
#include <concepts>

namespace Chisaka::Concepts{
  template<typename E>
  concept MemoryMapEntry = requires(E e){
    {e.base} -> std::convertible_to<std::size_t>;
    {e.length} -> std::convertible_to<std::size_t>;
    e.type;
  };

  template<typename M>
  concept MemoryMap = requires(M m, std::size_t i){
    typename M::Entry_t;
    typename M::Entry_t::Type;
    {m.Entries()} -> std::convertible_to<std::size_t>;
    m.Entry(i);
  };
}
