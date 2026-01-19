#include "allocator.hpp"

namespace{
  using namespace Chisaka::Slab;
  Buffer   buffer64;
  Buffer*  lowAddrBuffer;  
}

namespace Chisaka::Slab::Ns::Allocator{

Buffer& Buffer64(){ return buffer64;}
Buffer*& LowBuffer(){ return lowAddrBuffer;}

void Initialise(){
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

void Initialise(void* baseAddr){
  Buffer64().SetBufferSize(64);
  InitialiseLists(baseAddr);
  InitialiseLowBuffer();
}

void InitialiseLists(){
  void* baseAddr = Kernel::palloc();
  kout << "BaseAddr:" << reinterpret_cast<std::uintptr_t>(baseAddr) << '\n';
  InitialiseLists(baseAddr);
}

void InitialiseLists(void* initialAddr){
  ListDescriptor firstList64{initialAddr, 64, 1};
  firstList64.SetupLinkage();
  ListDescriptor* onList64 = 
    reinterpret_cast<ListDescriptor*>(firstList64.Allocate());
  *onList64 = std::move(firstList64);
  kassert(onList64 == initialAddr);
  Buffer64().AddList(onList64);
}

void InitialiseLowBuffer(){
  LowBuffer() = new(Buffer64().Allocate()) Buffer{64, 1};
}

//-------------------------------------------------------------

void AddBuffer(Buffer* buffer){
  Buffer64().AddBuffer(buffer);
}

void AddListDescriptor(ListDescriptor* ld){
  Buffer* buf = GetBuffer(ld->BufferSize());
  if(!buf){ 
    return;
  }
  else{
    buf->AddList(ld);
  }
}

void RemoveBuffer(Buffer* buffer){
  buffer->~Buffer();
}

//-------------------------------------------------------------

Buffer* GetBuffer(std::uint16_t size){
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

bool BufferExists(std::uint16_t size){
  if(GetBuffer(size)){
    return true;
  }
  else{
    return false;
  }
}

Buffer* NewBuffer(std::uint16_t bufferSize){
  if(BufferExists(bufferSize)){
    return nullptr;
  }
  // no existing buffer of the type
  Buffer* pbuffer = new(Buffer64().Allocate()) Buffer{bufferSize};
  AddBuffer(pbuffer);
  return pbuffer;
}

ListDescriptor* NewListDescriptor(std::uint16_t bufferSize, 
                                  std::uint8_t pages
){
  // get the next pow of 2 for the buffer size
  void* baseAddr = Kernel::palloc();
  ListDescriptor* list = 
    new(Buffer64().Allocate()) ListDescriptor{baseAddr, bufferSize, pages};
  list->SetupLinkage();
  AddListDescriptor(list);
  return list;
}

ListDescriptor* NewListDescriptor(void* baseAddr, 
                                  std::uint16_t bufferSize, 
                                  std::uint8_t pages
){
  // get the next pow of 2 for the buffer size
  ListDescriptor* list = 
    new(Buffer64().Allocate()) ListDescriptor{baseAddr, bufferSize, pages};
  list->SetupLinkage();
  AddListDescriptor(list);
  return list;
}

void* Allocate(std::size_t bytes){
  Buffer* buf = GetBuffer(bytes);
  if(buf){
    return buf->Allocate();
  }
  else{
    Buffer* pbuf = NewBuffer(bytes);
    return pbuf->Allocate();
  }
}

void Deallocate(void* pobj){
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

} // namespace Mem::Heap::Allocator
