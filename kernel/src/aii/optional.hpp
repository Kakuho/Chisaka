#pragma once

// optional allows nullable value type semantics

#include <expected>

#include <type_traits>

namespace Aii{

struct NullOptType{};
static inline constexpr NullOptType NullOpt{};

template<typename T>
class Optional{
  public:;
    constexpr Optional() noexcept;
    constexpr Optional(NullOptType) noexcept;
    constexpr Optional(const Optional& src) noexcept;
    constexpr Optional(const T& src) noexcept;
    constexpr Optional(Optional&& src) noexcept;
    constexpr ~Optional() noexcept = default;

    constexpr const T* operator->() const noexcept{return &m_val;}
    constexpr T* operator->() noexcept{return &m_val;}

    constexpr const T& operator*() const noexcept{return m_val;}
    constexpr T& operator*() noexcept{return m_val;}

    constexpr explicit operator bool() const noexcept{return HasVal();}
    constexpr bool HasVal() const noexcept{return !m_empty;}

    constexpr const T& Val() const noexcept{return m_val;}
    constexpr T& Val() noexcept{return m_val;}

    template<typename U = std::remove_cv_t<T>> 
    T ValOr(U&& defaultVal) const;

    constexpr void Reset() noexcept;

  private:
    union{
      char m_dummy;
      T m_val;
    };
    bool m_empty;
};

} // namespace Aii

template<typename T>
constexpr Aii::Optional<T>::Optional() noexcept
  :
    m_dummy{0},
    m_empty{true}
{

}

template<typename T>
constexpr Aii::Optional<T>::Optional(Aii::NullOptType) noexcept
  :
    m_dummy{0},
    m_empty{true}
{

}

template<typename T>
constexpr Aii::Optional<T>::Optional(const Aii::Optional<T>& src) noexcept{
  m_empty = src.m_empty;
  if(m_empty){
    m_dummy = 0;
  }
  else{
    m_val = src.m_val;
  }
}

template<typename T>
constexpr Aii::Optional<T>::Optional(const T& src) noexcept
  :
    m_val{src},
    m_empty{false}
{
  
}

template<typename T>
constexpr Aii::Optional<T>::Optional(Aii::Optional<T>&& src) noexcept{
  m_empty = src.m_empty;
  if(m_empty){
    m_dummy = 0;
  }
  else{
    m_val = src.m_val;
    src.m_val = 0;
  }
}
