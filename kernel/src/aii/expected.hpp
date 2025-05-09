#pragma once

// Wrapper for functions which are allowed to fail

#include <concepts>
#include <type_traits>
#include <utility>

namespace Aii{

template<typename T, typename E>
class Expected{
  public:
    using ExpectedType = T;
    using ErrorType = E;

    constexpr Expected() noexcept = delete;
    constexpr Expected(const Expected& src) noexcept;
    constexpr Expected(Expected&& src) noexcept;
    constexpr Expected(std::remove_cv_t<ExpectedType>&& src) noexcept;
    constexpr Expected(std::remove_cv_t<ErrorType>&& src) noexcept;
    constexpr ~Expected() noexcept;

    constexpr Expected& operator=(std::remove_cv_t<T> val){ m_val = val; return *this;}
    constexpr Expected& operator=(const Expected<T, E>& src)
    { m_val = src.m_val; return *this;}

    constexpr const T* operator->() const noexcept{ return &m_val;}
    constexpr T* operator->() noexcept{ return &m_val;}

    constexpr const T& operator*() const noexcept{ return m_val;}
    constexpr T& operator*() noexcept{ return m_val;}

    constexpr explicit operator bool() const noexcept{ return m_isVal;}
    constexpr bool HasVal() const noexcept{ return m_isVal;}

    constexpr const ExpectedType& Val() const noexcept{ return m_val;}
    constexpr ExpectedType& Val() noexcept{ return m_val;}

    constexpr const ErrorType& Error() const noexcept{ return m_error;}
    constexpr ErrorType& Error() noexcept{ return m_error;}

    constexpr Expected ValueOr(std::remove_cv_t<ExpectedType>&& dval) const;
    constexpr Expected ErrorOr(std::remove_cv_t<ErrorType>&& dval) const;

  private:
    union{
      ExpectedType m_val;
      ErrorType m_error;
    };
    bool m_isVal;

};

template<typename E>
class Expected<void, E>{
  public:
    using ExpectedType = void;
    using ErrorType = E;

    Expected() noexcept = delete;
    Expected(const Expected& src) noexcept;
    Expected(Expected&& src) noexcept;
    Expected(std::remove_cv_t<ErrorType>&& src) noexcept;
    ~Expected() noexcept;

    constexpr Expected& operator=(const Expected& src);

    constexpr void operator*() const noexcept{ return;}

    constexpr explicit operator bool() const noexcept{ return m_isVal;}
    constexpr bool HasVal() const noexcept{ return m_isVal;}

    constexpr void Val() const noexcept{ return;}

    constexpr const ErrorType& Error() const noexcept{ return m_error;}
    constexpr ErrorType& Error() noexcept{ return m_error;}

  private:
    union{
      char m_dummy;
      ErrorType m_error;
    };
    bool m_isVal;
};

} // namespace Aii

// Primary Template Aii::Expected<T, E>

template<typename T, typename E>
constexpr Aii::Expected<T, E>::Expected(const Expected& src) noexcept{
  m_isVal = src.m_isVal;
  if(m_isVal){
    m_val = src.m_val;
  }
  else{
    m_error = src.m_error;
  }
}

template<typename T, typename E>
constexpr Aii::Expected<T, E>::Expected(Expected&& src) noexcept{
  m_isVal = src.m_isVal;
  if(m_isVal){
    m_val = std::move(src.m_val);
  }
  else{
    m_error = std::move(src.m_error);
  }
}


template<typename T, typename E>
constexpr Aii::Expected<T, E>::Expected(std::remove_cv_t<ExpectedType>&& src) noexcept
  :
    m_val{src},
    m_isVal{true}
{

}

template<typename T, typename E>
constexpr Aii::Expected<T, E>::Expected(std::remove_cv_t<ErrorType>&& src) noexcept
  :
    m_error{src},
    m_isVal{false}
{

}

template<typename T, typename E>
constexpr Aii::Expected<T, E>::~Expected() noexcept{
  if(HasVal()){
    m_val.~T();
  }
  else{
    m_error.~E();
  }
}

// Partial Specialisation Template Aii::Expected<void, E>

template<typename E>
Aii::Expected<void, E>::Expected(const Expected& src) noexcept{
  m_isVal = src.m_isVal;
  if(m_isVal){
    m_dummy = src.m_dummy;
  }
  else{
    m_error = src.m_error;
  }
}

template<typename E>
Aii::Expected<void, E>::Expected(Expected&& src) noexcept{
  m_isVal = src.m_isVal;
  if(m_isVal){
    m_dummy = std::move(src.m_dummy);
  }
  else{
    m_error = std::move(src.m_error);
  }
}

template<typename E>
Aii::Expected<void, E>::Expected(std::remove_cv_t<ErrorType>&& src) noexcept
  :
    m_error{src},
    m_isVal{false}
{

}

template<typename E>
Aii::Expected<void, E>::~Expected() noexcept{
  if(HasVal()){
  }
  else{
    m_error.~E();
  }
}
