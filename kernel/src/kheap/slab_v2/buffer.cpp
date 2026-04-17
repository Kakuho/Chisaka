#include "buffer.hpp"
#include "allocator.hpp"

namespace Chisaka::Slab::v2{
    
Buffer::Buffer():
  m_lists{nullptr},
  m_bufferSize{0},
  m_options{0},
  m_opCount{0}
{

}

Buffer::Buffer(std::uint16_t bufferSize):
  m_lists{nullptr},
  m_bufferSize{bufferSize},
  m_options{0},
  m_opCount{0}
{

}

Buffer::~Buffer(){
  // return the slabs back to the page allocator
}

bool Buffer::Empty(){
  for(ListDescriptor& l: *m_lists){
    if(!l.Empty()){
      return false;
    }
  }
  return true;
}

bool Buffer::Full(){
  for(ListDescriptor& l: *m_lists){
    if(!l.Full()){
      return false;
    }
  }
  return true;
}

void Buffer::AddList(ListDescriptor* list){
  kassert(m_bufferSize == list->BufferSize());
  m_opCount++;
  m_lists->AddList(list);
}

bool Buffer::OwnsList(ListDescriptor* list){
  if(m_bufferSize == list->BufferSize()){
    return false;
  }
  for(ListDescriptor& l: *m_lists){
    if(&l == list){
      return true;
    }
  }

  m_opCount++;
  return false;
}

void Buffer::RemoveList(ListDescriptor* list){
  kassert(m_bufferSize == list->BufferSize());
  if(OwnsList(list)){
    list->ExtractSelf();
  }
  m_opCount++;
}

bool Buffer::OwnsObject(void* pobj){
  for(ListDescriptor& l: *m_lists){
    if(l.OwnsObject(pobj)){
      return true;
    }
  }
  return false;
}

void* Buffer::Allocate(){
  if(!m_lists){
    return nullptr;
  }
  void* alloc = nullptr;
  for(ListDescriptor& l: *m_lists){
    if(l.HasSpace()){
      alloc = l.Allocate();
    }
  }

  m_opCount++; 
  return alloc;
}

void Buffer::Deallocate(void* obj){
  for(ListDescriptor& l: *m_lists){
    if(l.OwnsObject(obj)){
      l.Deallocate(obj);
    }
  }
  m_opCount++; 
}
    
}
