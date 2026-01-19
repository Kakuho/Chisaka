#pragma once

#include <concepts>

namespace Chisaka::Concepts{
  template<typename S>
  concept KGlobalObject = requires(S s){
    {S::Get()} -> std::same_as<S&>;
  };
}
