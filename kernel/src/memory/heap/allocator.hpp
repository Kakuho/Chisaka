#pragma once

#include <cstdint>
#include <new>

#include "list_descriptor.hpp"
#include "buffer.hpp"
#include "options.hpp"

#include "aii/array.hpp"
#include "palloc.hpp"

namespace Mem::Heap::Allocator{
  void Initialise();
  void Initialise(void* baseAddr);

  void InitialiseLists();
  void InitialiseLists(void* initialAddr);
  void InitialiseLowBuffer();

  Mem::Heap::Buffer& Buffer64();
  Mem::Heap::Buffer*& LowBuffer();

  void AddBuffer(Mem::Heap::Buffer* buffer);             
  void AddListDescriptor(Mem::Heap::ListDescriptor* ld); 
  void RemoveBuffer(Mem::Heap::Buffer* buffer);               // still requires impl

  // would be nice to have std::optional<Buffer*> as return type
  Mem::Heap::Buffer* GetBuffer(std::uint16_t size);
  bool BufferExists(std::uint16_t size);

  Mem::Heap::Buffer* NewBuffer(std::uint16_t bufferSize);
  Mem::Heap::ListDescriptor* NewListDescriptor( std::uint16_t bufferSize, 
                                                std::uint8_t pages);
  Mem::Heap::ListDescriptor* NewListDescriptor(void* baseaddr, std::uint8_t bufferSize, 
                                               std::uint8_t pages);

  void* Allocate(std::size_t bytes);
  void Deallocate(void* pobj);

  // Todo:
  //  NewBuffer from low memory address
  //  Allocate from low memory address
  //  Deallocate from low memory address
}
