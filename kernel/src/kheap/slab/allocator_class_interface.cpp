#include "allocator_class_interface.hpp"
#include "palloc.hpp"

namespace Chisaka::Slab{

void Allocator::Init(){
  Buffer64().SetBufferSize(64);
  InitialiseLists();
  InitialiseLowBuffer();
  
  kout << reinterpret_cast<std::uintptr_t>(
    Buffer64().PartialListHead()->NextFreeBuffer()) << '\n';

  kassert(Buffer64().BufferSize() == 64);
  kassert(Buffer64().PartialListHead() != nullptr);

  kassert(Buffer64().FreeListHead() == nullptr);
  kassert(Buffer64().FullListHead() == nullptr);
  kassert(Buffer64().NextBuffer() == &Buffer64());
  kassert(Buffer64().PrevBuffer() == &Buffer64());
}

void Allocator::Init(void* baseAddr){
  m_buffer64.SetBufferSize(64);
  InitialiseLists(baseAddr);
  InitialiseLowBuffer();
}

void Allocator::InitialiseLists(){
  void* baseAddr = Kernel::palloc();
  kout << "BaseAddr:" << reinterpret_cast<std::uintptr_t>(baseAddr) << '\n';
  InitialiseLists(baseAddr);
}

void Allocator::InitialiseLists(void* initialAddr){
  ListDescriptor firstList64{initialAddr, 64, 1};
  firstList64.SetupLinkage();
  ListDescriptor* onList64 = 
    reinterpret_cast<ListDescriptor*>(firstList64.Allocate());
  *onList64 = std::move(firstList64);
  kassert(onList64 == initialAddr);
  Buffer64().AddList(onList64);
}

void Allocator::InitialiseLowBuffer(){
  LowBuffer() = new(Buffer64().Allocate()) Buffer{64, 1};
}

//-------------------------------------------------------------

void Allocator::AddBuffer(Buffer* buffer){
  Buffer64().AddBuffer(buffer);
}

void Allocator::AddListDescriptor(ListDescriptor* ld){
  Buffer* buf = GetBuffer(ld->BufferSize());
  if(!buf){ 
    return;
  }
  else{
    buf->AddList(ld);
  }
}

void Allocator::RemoveBuffer(Buffer* buffer){
  buffer->~Buffer();
}

//-------------------------------------------------------------

Buffer* Allocator::GetBuffer(std::uint16_t size){
  // very slow loop
  auto it = Buffer64().begin();
  do{
    if(it->BufferSize() == size){
      return it;
    }
    it = it->next();
  } while(it != Buffer64().end());
  return nullptr;
}

bool Allocator::BufferExists(std::uint16_t size){
  if(GetBuffer(size)){
    return true;
  }
  else{
    return false;
  }
}

Buffer* Allocator::NewBuffer(std::uint16_t bufferSize){
  if(BufferExists(bufferSize)){
    return nullptr;
  }
  // no existing buffer of the type
  Buffer* pbuffer = new(Buffer64().Allocate()) Buffer{bufferSize};
  AddBuffer(pbuffer);
  return pbuffer;
}

ListDescriptor* Allocator::NewListDescriptor(std::uint16_t bufferSize, std::uint8_t pages){
  // get the next pow of 2 for the buffer size
  void* baseAddr = Kernel::palloc();
  ListDescriptor* list = 
    new(Buffer64().Allocate()) ListDescriptor{baseAddr, bufferSize, pages};
  list->SetupLinkage();
  AddListDescriptor(list);
  return list;
}

ListDescriptor* Allocator::NewListDescriptor(void* baseAddr, std::uint16_t bufferSize, std::uint8_t pages){
  // get the next pow of 2 for the buffer size
  ListDescriptor* list = 
    new(Buffer64().Allocate()) ListDescriptor{baseAddr, bufferSize, pages};
  list->SetupLinkage();
  AddListDescriptor(list);
  return list;
}

void* Allocator::AllocateObject(std::size_t bytes){
  Buffer* buf = GetBuffer(bytes);
  if(buf){
    return buf->Allocate();
  }
  else{
    Buffer* pbuf = NewBuffer(bytes);
    return pbuf->Allocate();
  }
}

void Allocator::DeallocateObject(void* pobj){
  // would be much better performance with a page map so list descriptor lookup
  // is Theta(1) instead of Theta(N*M) where N = #buffer and M = #listdescriptors...
  for(Buffer& buf: Buffer64()){
    for(ListDescriptor& ld: *buf.FullListHead()){
      if(ld.ContainsObject(pobj)){
        ld.Deallocate(pobj);
      }
    }
    for(ListDescriptor& ld: *buf.PartialListHead()){
      if(ld.ContainsObject(pobj)){
        ld.Deallocate(pobj);
      }
    }
  }
}

}
