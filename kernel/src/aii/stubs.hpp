#pragma once

//  namespace Aii::Details holds all the stubs necessary for Aii to work.
//  By default they do nothing, but spin. 
//
//  If you want to use the full capacity of Aii, you must implement these functions
//  according to your own kernel

namespace Aii::Details{
 
  inline void AssertError(){
    while(true){;;}
  }

  template<typename T>
  void Delete(T* t){
    AssertError();
  }

  template<typename T, typename ...Args>
  T* Allocate(Args ...args){
    // perfect forward the argument to the allocation function
    AssertError();
  }

} // namespace Aii::Details
