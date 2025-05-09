#pragma once

// Smart pointer class representing unique ownership of a heap allocated object.
// For now it does not support array types

#include <cstddef>
#include <type_traits>
#include <utility>

#include "aii/stubs.hpp"
#include "default_delete.hpp"

namespace Aii{

template<typename T, typename D = DefaultDelete<T>>
class UniquePtr{
  using PointerType = T*;
  using LRefType = typename std::add_lvalue_reference<T>::type;
  public:
    constexpr UniquePtr() noexcept;
    constexpr UniquePtr(std::nullptr_t) noexcept;
    constexpr UniquePtr(PointerType pointer) noexcept;
    constexpr UniquePtr(const UniquePtr& src) noexcept = delete;
    constexpr UniquePtr(UniquePtr&& src) noexcept;
    constexpr UniquePtr& operator=(std::nullptr_t) noexcept;
    constexpr UniquePtr& operator=(UniquePtr&& src) noexcept;
    constexpr UniquePtr& operator=(const UniquePtr& src) noexcept = delete;
    constexpr ~UniquePtr() noexcept;

    constexpr explicit operator bool() const noexcept{ return m_ptr;}

    [[nodiscard]] PointerType Release() noexcept;
    void Reset() noexcept;

    [[nodiscard]] PointerType Get() const noexcept{ return m_ptr;}
    [[nodiscard]] D& GetDeleter() const noexcept;

    LRefType operator*() const noexcept{ return *m_ptr;}
    PointerType operator->() const noexcept{ return m_ptr;}

  private:
    T* m_ptr;
    D m_deleter;
};

template<typename T, typename ...Args>
UniquePtr<T> MakeUnique(Args ...args);

template<typename T>
UniquePtr<T> MakeUniqueForOverwrite();


} // namespace Aii

template<typename T, typename D>
constexpr Aii::UniquePtr<T, D>::UniquePtr() noexcept
  :
    m_ptr{nullptr}
{

}

template<typename T, typename D>
constexpr Aii::UniquePtr<T, D>::UniquePtr(std::nullptr_t) noexcept
  :
    m_ptr{nullptr}
{

}

template<typename T, typename D>
constexpr Aii::UniquePtr<T, D>::UniquePtr(PointerType pointer) noexcept
  :
    m_ptr{pointer}
{

}

template<typename T, typename D>
constexpr Aii::UniquePtr<T, D>::UniquePtr(UniquePtr&& src) noexcept
  :
    m_ptr{src.m_ptr}
{

  src.m_ptr = nullptr;
}

template<typename T, typename D>
constexpr Aii::UniquePtr<T, D>& 
Aii::UniquePtr<T,D>::operator=(UniquePtr&& src) noexcept{
  if(this == &src){
    return *this;
  }
  else{
    m_ptr = src.m_ptr;
    src.m_ptr = nullptr;
    return *this;
  }
}

template<typename T, typename D>
constexpr Aii::UniquePtr<T, D>& 
Aii::UniquePtr<T, D>::operator=(std::nullptr_t) noexcept{
}

template<typename T, typename D>
constexpr Aii::UniquePtr<T, D>::~UniquePtr() noexcept{
  m_deleter(m_ptr);
}

template<typename T, typename D>
[[nodiscard]] auto 
Aii::UniquePtr<T, D>::Release() noexcept -> PointerType{
  T* tmp = m_ptr;
  m_ptr = nullptr;
  return tmp;
}

template<typename T, typename D>
void Aii::UniquePtr<T, D>::Reset() noexcept{
  auto old = m_ptr;
  if(old){
    GetDeleter()(m_ptr);
  }
}

template<typename T, typename D>
[[nodiscard]] D&
Aii::UniquePtr<T, D>::GetDeleter() const noexcept{
  return m_deleter;
}

template<typename T, typename ...Args>
Aii::UniquePtr<T> Aii::MakeUnique(Args ...args){
  return UniquePtr<T>(Details::Allocate<T>(std::forward<Args>(args)...));
}

template<typename T>
Aii::UniquePtr<T> Aii::MakeUniqueForOverwrite(){
  return UniquePtr<T>(Details::Allocate<T>());
}
