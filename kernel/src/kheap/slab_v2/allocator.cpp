#include "allocator.hpp"

namespace Chisaka::Slab::v2{

void Allocator::Init(PageAllocation_fn alloc, PageDeallocation_fn dealloc){
  m_palloc = alloc;
  m_pdealloc = dealloc;
  InitBuffers();
}

void Allocator::InitBuffers(){
  m_buffers[0] = Buffer{8};
  m_buffers[1] = Buffer{16};
  m_buffers[2] = Buffer{32};
  m_buffers[3] = Buffer{64};
  m_buffers[4] = Buffer{128};
  m_buffers[5] = Buffer{256};
  m_buffers[6] = Buffer{512};
  m_buffers[7] = Buffer{1024};
  m_buffers[8] = Buffer{2048};
}

Buffer& Allocator::GetBuffer(std::uint16_t size){
  return m_buffers[Pow2ToIndex(NbytesToPow2(size))];
}

void Allocator::AddList(ListDescriptor* list){
  auto& buffer = GetBuffer(list->BufferSize());
  buffer.AddList(list);
}

ListDescriptor* Allocator::AllocateList(void* pagebase, std::uint16_t bufferSize){
  // get the next pow of 2 for the buffer size
  auto& buffer = GetBuffer(sizeof(ListDescriptor));
  if(buffer.Full()){
    //  the buffer is full and has no more space to allow for allocations, so we allocate another
    //  list descriptor to the buffer which contains class ListDescriptor
    void* extrapage = m_palloc();
    ListDescriptor list{extrapage, NbytesToPow2(sizeof(ListDescriptor))};
    ListDescriptor* onpageList = static_cast<ListDescriptor*>(list.Allocate());
    *onpageList = std::move(list);  // move the list descriptor on page
    buffer.AddList(onpageList);
  }
  void* allocd = buffer.Allocate();
  ListDescriptor* list = new (allocd) ListDescriptor{pagebase, bufferSize};
  list->SetupLinkage();
  AddList(list);
  return list;
}

void Allocator::DeallocateList(ListDescriptor* list){
  auto& buffer = GetBuffer(NbytesToPow2(list->BuffersUsed()));
  buffer.RemoveList(list);
  m_pdealloc(reinterpret_cast<void*>(list->Base()));
}

void* Allocator::AllocateObject(std::size_t bytes){
  std::size_t pow2 = NbytesToPow2(bytes);
  Buffer& buffer = GetBuffer(pow2);
  if(buffer.Full()){
    void* pagebase = m_palloc();
    ListDescriptor* list = AllocateList(pagebase, bytes);
    buffer.AddList(list);
  }
  void* allocd = buffer.Allocate();
  return allocd;
}

void Allocator::DeallocateObject(void* pobj, std::size_t bytes){
  std::size_t pow2 = NbytesToPow2(bytes);
  Buffer& buffer = GetBuffer(pow2);
  buffer.Deallocate(pobj);
}

}
