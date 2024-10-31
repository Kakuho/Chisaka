#ifndef MEMORY_ALIGNMENT_HPP 
#define MEMORY_ALIGNMENT_HPP

// Functions to perform memory address alignment.

#include <cstdint>
#include <concepts>

namespace Mem{

  // Sensible type declarations
  //  kvirtaddr_t is the kernel's virtual address
  //  (different processes may have different virtual address mappings)

  using physaddr_t = std::uint64_t;
  using kvirtaddr_t = std::uint64_t;

  // ------------------------------------------------------ //
  //  Aligning Functions
  // ------------------------------------------------------ //

  template<typename T>
  concept IsAddressType = (
                            std::same_as<T, physaddr_t> || 
                            std::same_as<T, kvirtaddr_t>
                          );

  template<typename T, std::size_t AlignAddress>
    requires IsAddressType<T>
  constexpr T Align(T addr){
    return  (addr % AlignAddress) == addr ? 
            addr : 
            (addr % AlignAddress) + AlignAddress;
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
  constexpr T NearestAlign(T addr){
    T aligned = addr % AlignAddress;
    if(aligned > AlignAddress/2){
      return aligned + AlignAddress;
    }
    else{
      return aligned;
    }
  }

} // namespace Mem
  
#endif
