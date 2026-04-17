#include "list_descriptor.hpp"
#include <cstdint>

namespace Chisaka::Slab::v2{

ListDescriptor::ListDescriptor(
    void* baseAddr, 
    std::uint16_t bufferSize
):
  m_baseAddress{baseAddr},
  m_nextFree{nullptr},
  m_nextList{this},
  m_prevList{this},
  m_bufferSize{bufferSize < 8 ? static_cast<std::uint16_t>(8) : bufferSize},
  m_buffersUsed{0}
{
  m_totalBuffers = PAGE_SIZE / m_bufferSize;
  kassert(m_bufferSize >= 8);
  kassert(m_bufferSize <= 2048);
}

ListDescriptor::~ListDescriptor(){
  ExtractSelf();
  // returns the pages back to the page allocator
}

void ListDescriptor::PoisonFreeArea(std::uint8_t val){
  for(
      std::uint8_t* currentByte = reinterpret_cast<std::uint8_t*>(Base()); 
      reinterpret_cast<std::uint64_t>(currentByte) < Limit(); 
      currentByte++
  ){
    *currentByte = val;
  }
}

void ListDescriptor::SetupLinkage(){
  // Basic Singly Linked List Linkage
  std::uint64_t startAddress = Base();
  std::uintptr_t limit = Limit(); 
  ListEntry* currentEntry = reinterpret_cast<ListEntry*>(startAddress);
  m_nextFree = currentEntry;
  while(reinterpret_cast<std::uint64_t>(currentEntry) < limit)
  {
    currentEntry->next = reinterpret_cast<ListEntry*>(
        reinterpret_cast<std::uint64_t>(currentEntry) + BufferSize()
    );
    currentEntry = currentEntry->next;
  }
  currentEntry->next = nullptr;
}

std::uintptr_t ListDescriptor::Limit() const{
  return Base() + PAGE_SIZE;
}

std::uint16_t ListDescriptor::InitialFreeSpace() const{
  return Limit() - Base();
}

std::uint16_t ListDescriptor::SpaceAfterAllocation() const{
  return InitialFreeSpace() - TotalBuffers() * BufferSize();
}

bool ListDescriptor::ObjectInRange(void* obj) const{
  std::uintptr_t objaddr = reinterpret_cast<std::uintptr_t>(obj);
  return (objaddr >= Base()) && (objaddr < Limit());
}

void* ListDescriptor::Allocate(){
  if(!m_nextFree){
    return nullptr;
  }
  ListEntry* head = m_nextFree;
  void* allocd = reinterpret_cast<void*>(head);
  m_nextFree = m_nextFree->next;
  m_buffersUsed++;
  return allocd;
}

void ListDescriptor::Deallocate(void* address){
  // Todo: Make sure the address is valid within the slab
  // Preliminary Checks
  if(!ObjectInRange(address) || m_buffersUsed == 0){
    return;
  }
  if(!m_nextFree){ 
    // the list descriptor is empty
    m_nextFree = reinterpret_cast<ListEntry*>(address);
    m_nextFree->next = nullptr;
    m_buffersUsed--;
    return;
  }
  // Core logic: loop through and place at the end of the freelist
  ListEntry* allocd = reinterpret_cast<ListEntry*>(address);
  ListEntry* currentEntry = m_nextFree;
  ListEntry* nextEntry = m_nextFree->next;
  while(nextEntry){
    currentEntry = nextEntry;
    nextEntry = currentEntry->next;
  }
  currentEntry->next = allocd;
  allocd->next = nullptr;;
  m_buffersUsed--;
}

// Linked List API

void ListDescriptor::AddList(ListDescriptor* src){
  // kinda want to add more false tolerance...
  if(src->BufferSize() != m_bufferSize){
    return;
  }
  if(this == src){
    // self assignment
    return;
  }
  else if((this == PrevList()) && (this == NextList())){
    // size 1 list
    m_nextList = src;
    m_prevList = src;
    src->m_prevList = this;
    src->m_nextList = this;
  }
  else{
    // size >= 2 list
    src->m_nextList = this;
    src->m_prevList = this->m_prevList;
    m_prevList->m_nextList = src;
    m_prevList = src;
  }
}
    
void ListDescriptor::Remove(ListDescriptor* entry){
  if(this == entry){
    return;
  }
  // Size 2 List
  if((PrevList() == entry) && (NextList() == entry)){
    m_nextList = this;
    m_prevList = this;
    return;
  }
  // Size >= 3
  entry->PrevList()->NextList() = entry->NextList();
  entry->NextList()->PrevList() = entry->PrevList();
  return;
}

ListDescriptor* ListDescriptor::Extract(ListDescriptor* entry){
  // Extract is similiar to Remove, but you get the list descriptor 
  if(this == entry){
    return nullptr;
  }
  // Size 2 List
  else if((PrevList() == entry) && (NextList() == entry)){
    m_nextList = this;
    m_prevList = this;
    return entry;
  }
  else{
    // Size >= 3
    entry->PrevList()->NextList() = entry->NextList();
    entry->NextList()->PrevList() = entry->PrevList();
    return entry;
  }
}

ListDescriptor* ListDescriptor::ExtractSelf(){
  // at the end of this, the next element of the list is the list head,
  // with the correct linkages 
  if(NextList() == this && PrevList() == this){
    return this;
  }
  else if(NextList() == PrevList()){
    // Size 2 Free List
    ListDescriptor* newHead = NextList();
    newHead->NextList() = newHead;
    newHead->PrevList() = newHead;
  }
  else{
    ListDescriptor* newHead = NextList();
    this->m_prevList->m_nextList = newHead;
    newHead->m_prevList = this->m_prevList;
  }
  // fix up its own linkages
  this->NextList() = this;
  this->PrevList() = this;
  return this;
}

}
