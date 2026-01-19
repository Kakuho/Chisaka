#pragma once

// main frontend to the slab heap allocator

#include <cstdint>
#include <new>

#include "kassert.hpp"
#include "drivers/serial/kostream.hpp"

#include "list_descriptor.hpp"
#include "buffer.hpp"
#include "options.hpp"

#include "aii/array.hpp"
#include "palloc.hpp"

namespace Chisaka::Slab{

class Allocator{
  public:
    static Allocator& Get(){ static Allocator alc; return alc;}
    void Init();
    void Init(void* baseAddr);

    Buffer& Buffer64(){return m_buffer64;}
    Buffer*& LowBuffer(){return m_lowAddrBuffer;}

    void AddBuffer(Buffer* buffer);             
    void AddListDescriptor(ListDescriptor* ld); 
    void RemoveBuffer(Buffer* buffer);               // still requires impl

    // would be nice to have std::optional<Buffer*> as return type
    Buffer* GetBuffer(std::uint16_t size);
    bool BufferExists(std::uint16_t size);

    Buffer* NewBuffer(std::uint16_t bufferSize);
    ListDescriptor* NewListDescriptor(std::uint16_t bufferSize, std::uint8_t pages);
    ListDescriptor* NewListDescriptor(void* baseaddr, std::uint16_t bufferSize, std::uint8_t pages);

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

  private:
    void InitialiseLists();
    void InitialiseLists(void* initialAddr);
    void InitialiseLowBuffer();

    Buffer   m_buffer64;
    Buffer*  m_lowAddrBuffer;  
};

}
