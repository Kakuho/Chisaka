#include "list_descriptor.hpp"

namespace Mem::Heap{

ListDescriptor::ListDescriptor(
    void* baseAddr, 
    std::uint16_t bufferSize, 
    std::uint8_t pages
):
  m_baseAddress{baseAddr},
  m_nextFree{nullptr},
  m_nextList{this},
  m_prevList{this},
  m_bufferSize{bufferSize},
  m_buffersUsed{0},
  m_pages{pages}
{
  m_totalBuffers = (m_pages * 0x1000) / m_bufferSize;
  kassert(m_bufferSize >= 8);
}

ListDescriptor& ListDescriptor::operator=(ListDescriptor&& src){
  this->m_baseAddress = src.m_baseAddress;
  this->m_nextFree = src.m_nextFree;
  this->m_bufferSize = src.m_bufferSize;
  this->m_totalBuffers = src.m_totalBuffers;
  this->m_buffersUsed = src.m_buffersUsed;
  this->m_pages = src.m_pages;

  // linked list

  this->m_nextList = src.m_nextList;
  if(this->m_nextList){
    src.m_nextList->m_prevList = this;
  }

  this->m_prevList = src.m_prevList;
  if(this->m_prevList){
    src.m_prevList->m_nextList = this;
  }

  return *this;
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
  // Basic Free List Linkage
  std::uint64_t startAddress = Base();
  std::uintptr_t limit = Limit();
  std::uintptr_t** currentBuffer = reinterpret_cast<std::uintptr_t**>(startAddress);
  m_nextFree = currentBuffer;
  while(reinterpret_cast<std::uint64_t>(currentBuffer) + BufferSize() < limit)
  {
    *currentBuffer = reinterpret_cast<std::uintptr_t*>(
        reinterpret_cast<std::uint64_t>(currentBuffer) + BufferSize()
    );
    currentBuffer = reinterpret_cast<std::uintptr_t**>(
        reinterpret_cast<std::uint64_t>(currentBuffer) + BufferSize()
    );
  }
  *currentBuffer = nullptr;
}

std::uintptr_t ListDescriptor::Limit() const{
  return Base() + PAGE_SIZE * Pages();
}

std::uint16_t ListDescriptor::InitialFreeSpace() const{
  return Limit() - Base();
}

std::uint16_t ListDescriptor::SpaceAfterAllocation() const{
  return InitialFreeSpace() - TotalBuffers() * BufferSize();
}

void* ListDescriptor::Allocate(){
  if(m_buffersUsed == m_totalBuffers) return nullptr;
  std::uintptr_t* entry = reinterpret_cast<std::uintptr_t*>(m_nextFree);
  void* allocd = entry;
  m_nextFree = reinterpret_cast<void*>(*entry);
  m_buffersUsed++;
  if(m_buffersUsed == m_totalBuffers){
    m_nextFree = nullptr;
  }
  return allocd;
}

void ListDescriptor::Deallocate(void* address){
  // Todo: Make sure the address is valid within the slab
  // Preliminary Checks
  if(!ObjectInRange(address) || m_buffersUsed == 0){
    return;
  }
  if(m_nextFree == nullptr){ 
    m_nextFree = address; 
    *reinterpret_cast<std::uintptr_t*>(m_nextFree) = 0;
    m_buffersUsed--;
    return;
  }
  if(address > m_nextFree){
    *reinterpret_cast<std::uintptr_t*>(m_nextFree) 
      = reinterpret_cast<std::uintptr_t>(address);
    m_buffersUsed--;
    return;
  }
  // Core logic: iterate to find the cell where the operand sits
  std::uintptr_t* operandCell = reinterpret_cast<std::uintptr_t*>(address);
  std::uintptr_t* prevCell = nullptr;
  std::uintptr_t* currentCell = reinterpret_cast<std::uintptr_t*>(m_nextFree);
  while(operandCell > currentCell){
    prevCell = currentCell;
    currentCell = reinterpret_cast<std::uintptr_t*>(*currentCell);
  }
  if(!prevCell){
    *operandCell = reinterpret_cast<std::uintptr_t>(currentCell);
    m_nextFree = operandCell;
  }
  else{
    *prevCell = reinterpret_cast<std::uintptr_t>(operandCell);
    *operandCell = reinterpret_cast<std::uintptr_t>(currentCell);
  }
  m_buffersUsed--;
}

bool ListDescriptor::ObjectInRange(void* obj) const{
  std::uintptr_t objaddr = reinterpret_cast<std::uintptr_t>(obj);
  return (objaddr >= Base()) && (objaddr < Limit());
}

// Linked List API

void ListDescriptor::AddList(ListDescriptor* src){
  // kinda want to add more false tolerance...
  kassert(src->BufferSize() == m_bufferSize);
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
