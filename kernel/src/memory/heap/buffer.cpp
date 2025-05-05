#include "buffer.hpp"
#include "allocator.hpp"

namespace Mem::Heap{
    
Buffer::Buffer():
  m_freeLists{nullptr},
  m_fullLists{nullptr},
  m_partialLists{nullptr},
  m_nextBuffer{this},
  m_prevBuffer{this},
  m_bufferSize{0},
  m_options{0},
  m_opCount{0}
{

}

Buffer::Buffer(std::uint16_t bufferSize):
  m_freeLists{nullptr},
  m_fullLists{nullptr},
  m_partialLists{nullptr},
  m_nextBuffer{this},
  m_prevBuffer{this},
  m_bufferSize{bufferSize},
  m_options{0},
  m_opCount{0}
{

}

Buffer::Buffer(std::uint16_t bufferSize, std::uint8_t options){

}

Buffer::~Buffer(){
  ExtractSelf();
  // returns the slab pages back to the page allocator
}

void Buffer::AddList(ListDescriptor* list){
  // should we iterate to see if the list already belongs 
  // to the buffer?
  m_opCount++;
  kassert(list->BufferSize() == this->BufferSize());
  if(list->Full()){
    TryAddFullList(list);
  }
  else if(list->Empty()){
    TryAddFreeList(list);
  }
  else{
    TryAddPartialList(list);
  }
}

void Buffer::TryAddFreeList(ListDescriptor* list){
  if(m_freeLists){
    m_freeLists->AddList(list);
  }
  else{
    m_freeLists = list;
    m_freeLists->NextList() = list;
    m_freeLists->PrevList() = list;
  }
}

void Buffer::TryAddPartialList(ListDescriptor* list){
  if(m_partialLists){
    m_partialLists->AddList(list);
  }
  else{
    m_partialLists = list;
    m_partialLists->NextList() = list;
    m_partialLists->PrevList() = list;
  }
}

void Buffer::TryAddFullList(ListDescriptor* list){
  if(m_fullLists){
    m_fullLists->AddList(list);
  }
  else{
    m_fullLists = list;
    m_fullLists->NextList() = list;
    m_fullLists->PrevList() = list;
  }
}

void Buffer::MoveFreeHead(){
  if(!m_freeLists){ return;}
  if(m_freeLists->NextList() == m_freeLists){
    AddList(m_freeLists);
    m_freeLists = nullptr;
  }
  else{
    ListDescriptor* nextHead = m_freeLists->NextList();
    ListDescriptor* prevHead = m_freeLists->ExtractSelf();
    AddList(prevHead);
    m_freeLists = nextHead;

  }
}

void Buffer::MovePartialHead(){
  if(!m_partialLists){ return;}
  if(m_partialLists->NextList() == m_partialLists){
    AddList(m_partialLists);
    m_partialLists = nullptr;
  }
  else{
    ListDescriptor* nextHead = m_partialLists->NextList();
    ListDescriptor* prevHead = m_partialLists->ExtractSelf();
    AddList(prevHead);
    m_partialLists = nextHead;

  }
}

void Buffer::MoveFullHead(){
  if(!m_fullLists){ return;}
  if(m_fullLists->NextList() == m_fullLists){
    AddList(m_fullLists);
    m_fullLists = nullptr;
  }
  else{
    ListDescriptor* nextHead = m_fullLists->NextList();
    ListDescriptor* prevHead = m_fullLists->ExtractSelf();
    AddList(prevHead);
    m_fullLists = nextHead;
  }
}

void Buffer::MoveFreeEntry(ListDescriptor* entry){
  // results is undefined if entry is not an entry in the free list;
  if(!entry){ return;}
  if(entry == m_freeLists){ 
    MoveFreeHead(); 
    return;
  }
  m_freeLists->Extract(entry);
  AddList(entry);
}

void Buffer::MovePartialEntry(ListDescriptor* entry){
  // results is undefined if partialList is not an entry in the partial list;
  if(!entry){ return;}
  if(entry == m_partialLists){ 
    MovePartialHead(); 
    return;
  }
  m_partialLists->Extract(entry);
  AddList(entry);
}

void Buffer::MoveFullEntry(ListDescriptor* entry){
  // results is undefined if entry is not an entry in the full list;
  if(!entry){ return;}
  if(entry == m_fullLists){ 
    MoveFullHead(); 
    return;
  }
  m_fullLists->Extract(entry);
  AddList(entry);
}

void* Buffer::Allocate(){
  void* alloc = nullptr;
  if(m_partialLists){
    alloc = AddToPartial();
  }
  if(!alloc && m_freeLists){
    alloc = AddToFree();
  }
  if(!alloc){
    // grow and then allocate it
    Grow();
    alloc = AddToFree();
    kassert(alloc != nullptr);
  }
  m_opCount++;  // for gc
  return alloc;
}

void* Buffer::AddToFree(){
  if(!m_freeLists){
    return nullptr;
  }
  void* alloc = nullptr;
  alloc = m_freeLists->Allocate();
  MoveFreeHead();
  return alloc;
}

void* Buffer::AddToPartial(){
  if(!m_partialLists){
    return nullptr;
  }
  void* alloc = nullptr;
  auto it = m_partialLists->begin();
  do{
    if(it->HasSpace()){
      alloc = it->Allocate();
      if(it->Full() && it == m_partialLists){
        MovePartialHead();
      }
      else if(it->Full()){
        MovePartialEntry(it);
      }
      break;
    }
    it = m_partialLists->next();
  }while(it != m_partialLists->end());
  return alloc;
}

void Buffer::Deallocate(void* objaddr){
  // this is a matter of determining which list is the object in range for
  if(DeallocPartial(objaddr)){
    return;
  }
  else if(DeallocFull(objaddr)){
    return;
  }
  else{
    // ??
    return;
  }
  m_opCount++; // for gc
}

Buffer::DeStatus Buffer::DeallocPartial(void* objaddr){
  // O(n) where n = number of entries in the partial list
  bool status = false;
  if(!m_partialLists){ return status;}
  auto it = m_partialLists->begin();
  do{
    if(it->ObjectInRange(objaddr)){
      it->Deallocate(objaddr);
      status = true;
      if(it->Empty()){
        MovePartialEntry(it);
      }
      break;
    }
    it = it->next();
  }while(it != m_partialLists->end());
  return status;
}

Buffer::DeStatus Buffer::DeallocFull(void* objaddr){
  // O(n) where n = number of entries in the full list
  // Can be improved with a page map
  bool status = false;
  if(!m_fullLists){ return status;}
  auto it = m_fullLists->begin();

  do{
    if(it->ObjectInRange(objaddr)){
      it->Deallocate(objaddr);
      status = true;
      if(!it->Full()){
        MoveFullEntry(it);
      }
      break;
    }
    it = it->next();
  }while(it != m_fullLists->end());

  return status;
}

void Buffer::Grow(){
  // Grow create a new list descriptor
  ListDescriptor* list = Allocator::NewListDescriptor(m_bufferSize, 1);
  if(m_freeLists){
    m_freeLists->AddList(list);
  }
  else{
    m_freeLists = list;
  }
}

void Buffer::Reap(){
  // Finds free list descriptors to reap from
}

// Linked List API

void Buffer::AddBuffer(Buffer* src){
  // kinda want to add more false tolerance...
  if(this == src){
    // self assignment
    return;
  }
  else if((this == PrevBuffer()) && (this == NextBuffer())){
    // size 1 list
    m_nextBuffer = src;
    m_prevBuffer = src;
    src->m_prevBuffer = this;
    src->m_nextBuffer = this;
  }
  else{
    // size >= 2 list
    src->m_nextBuffer = this;
    src->m_prevBuffer = this->m_prevBuffer;
    m_prevBuffer->m_nextBuffer = src;
    m_prevBuffer = src;
  }
}
    
void Buffer::Remove(Buffer* entry){
  if(this == entry){
    return;
  }
  // Size 2 List
  if((PrevBuffer() == entry) && (NextBuffer() == entry)){
    m_nextBuffer = this;
    m_prevBuffer = this;
    return;
  }
  // Size >= 3
  entry->PrevBuffer()->NextBuffer() = entry->NextBuffer();
  entry->NextBuffer()->PrevBuffer() = entry->PrevBuffer();
  return;
}

Buffer* Buffer::Extract(Buffer* entry){
  // Extract is similiar to Remove, but you get the list descriptor 
  if(this == entry){
    return nullptr;
  }
  // Size 2 List
  else if((PrevBuffer() == entry) && (NextBuffer() == entry)){
    m_nextBuffer = this;
    m_prevBuffer = this;
    return entry;
  }
  else{
    // Size >= 3
    entry->PrevBuffer()->NextBuffer() = entry->NextBuffer();
    entry->NextBuffer()->PrevBuffer() = entry->PrevBuffer();
    return entry;
  }
}

Buffer* Buffer::ExtractSelf(){
  // at the end of this, the next element of the list is the list head,
  // with the correct linkages 
  if(NextBuffer() == this && PrevBuffer() == this){
    return this;
  }
  else if(NextBuffer() == PrevBuffer()){
    // Size 2 Free List
    Buffer* newHead = NextBuffer();
    newHead->NextBuffer() = newHead;
    newHead->PrevBuffer() = newHead;
  }
  else{
    Buffer* newHead = NextBuffer();
    newHead->PrevBuffer()->PrevBuffer()->NextBuffer() = newHead;
    newHead->PrevBuffer() = this->PrevBuffer();
  }
  // fix up its own linkages
  this->NextBuffer() = this;
  this->PrevBuffer() = this;
  return this;
}

}
