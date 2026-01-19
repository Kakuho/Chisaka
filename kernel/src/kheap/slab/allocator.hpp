#pragma once

#include <cstdint>
#include <new>

#include "list_descriptor.hpp"
#include "buffer.hpp"
#include "options.hpp"

#include "aii/array.hpp"
#include "palloc.hpp"

namespace Chisaka::Slab::Allocator{
  void Initialise();
  void Initialise(void* baseAddr);

  void InitialiseLists();
  void InitialiseLists(void* initialAddr);
  void InitialiseLowBuffer();

  Buffer& Buffer64();
  Buffer*& LowBuffer();

  void AddBuffer(Buffer* buffer);             
  void AddListDescriptor(ListDescriptor* ld); 
  void RemoveBuffer(Buffer* buffer);               // still requires impl

  // would be nice to have std::optional<Buffer*> as return type
  Buffer* GetBuffer(std::uint16_t size);
  bool BufferExists(std::uint16_t size);

  Buffer* NewBuffer(std::uint16_t bufferSize);
  ListDescriptor* NewListDescriptor( std::uint16_t bufferSize, 
                                                std::uint8_t pages);
  ListDescriptor* NewListDescriptor(void* baseaddr, std::uint8_t bufferSize, 
                                               std::uint8_t pages);

  void* Allocate(std::size_t bytes);
  void Deallocate(void* pobj);

  // heap management w.r.t types
  template<typename T>  
  T* New(){ 
    T* alloc = static_cast<T*>(Allocate(sizeof(T)));
    alloc = new(alloc) T;
    return alloc;
  }

  template<typename T>  
  void Delete(T* pobj){
    Deallocate(pobj);
  }

  // Todo:
  //  NewBuffer from low memory address
  //  Allocate from low memory address
  //  Deallocate from low memory address
}
