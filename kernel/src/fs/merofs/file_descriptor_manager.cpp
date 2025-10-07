#include <cstdint>

#include"file_descriptor_manager.hpp"

#include "aii/array.hpp"

namespace Fs::Merofs{

// File Descriptor Table Chunks

FileDescriptorTable::TableChunk::TableChunk()
  :
    next{nullptr},
    prev{nullptr},
    used{0},
    table{}
{

}

FileDescriptorTable::TableChunk::~TableChunk()
{
}

// File Descriptor Table

FileDescriptorTable::FileDescriptorTable()
  :
    m_root{Mem::Heap::Allocator::New<TableChunk>()},
    m_maxEntries{TableChunk::entries},
    m_full{false}
{

}

FileDescriptorTable::~FileDescriptorTable(){
  TableChunk* indexer = m_root;
  while(indexer->next){
    TableChunk* next = indexer->next;
    Mem::Heap::Allocator::Delete(indexer);
    indexer = next;
  }
}

void FileDescriptorTable::AllocateChunk(){
  auto allocchunk = Mem::Heap::Allocator::New<TableChunk>();
  if(m_root){
    m_root->next = allocchunk;
  }
}

const FileDescriptor* FileDescriptorTable::GetFd(std::uint8_t fd) const{
  TableChunk* tableIndexer = m_root;
  if(fd > TableEntries()){
    tableIndexer = tableIndexer->next;
    fd -= TableEntries();
  }
  return &tableIndexer->Entry(fd);
}

FileDescriptor* FileDescriptorTable::CreateNewFd(){
  TableChunk* tableIndexer = m_root;
  if(tableIndexer && !tableIndexer->Full()){
    std::uint8_t freeindex = tableIndexer->NextFree();
    tableIndexer->Entry(freeindex) = FileDescriptor{freeindex};
    return &tableIndexer->Entry(freeindex);
  }
  else{
    return nullptr;
  }
}

}
