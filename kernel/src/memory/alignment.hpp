#pragma once

// Functions to perform memory address alignment.

#include <cstdint>
#include <concepts>
#include <type_traits>
#include "types.hpp"

namespace Mem{

  template<typename T>
  concept IsAddressType =
    std::same_as<T, Chisaka::PhysAddr> || std::same_as<T, Chisaka::VirtAddr>;

  template<typename T>
  concept NotIntegralAddressType = !IsAddressType<T>;

  template<typename T>
  concept IsPointerType = std::is_pointer_v<T>;

  // ------------------------------------------------------ //
  //  Aligning Functions
  // ------------------------------------------------------ //

  template<typename T, std::size_t AlignAddress>
    requires IsAddressType<T>
  constexpr T Align(T addr){
    return  (addr % AlignAddress) == addr ? addr : 
            addr - (addr % AlignAddress);
  }
  
  template<typename T, std::size_t AlignAddress>
    requires IsPointerType<T>
  constexpr std::uint64_t Align(T ptr){
    std::uint64_t addr = reinterpret_cast<std::uint64_t>(ptr);
    return  (addr % AlignAddress) == addr ? addr : 
            addr - (addr % AlignAddress);
  }

  template<typename T, std::size_t AlignAddress>
    requires IsAddressType<T>
  constexpr T AlignUp(T addr){
    T aligned = addr % AlignAddress;
    if(addr > aligned){
      return aligned + AlignAddress;
    }
    else{
      return aligned;
    }
  }

  template<typename T, std::size_t AlignAddress>
    requires IsAddressType<T>
  constexpr auto NearestAlign(T addr){
    T aligned = addr % AlignAddress;
    if(aligned == 0){
      return addr;
    }
    if(aligned > AlignAddress/2){
      return aligned + AlignAddress;
    }
    else{
      return aligned;
    }
  }

  template<typename T, std::size_t AlignAddress>
    requires NotIntegralAddressType<T>
  constexpr auto NearestAlign(T addr){
    Chisaka::VirtAddr aligned = reinterpret_cast<Chisaka::VirtAddr>(addr) % 
                          AlignAddress;
    if(aligned == 0){
      return addr;
    }
  }

} // namespace Mem
