#include <cstdint>

#include"file_descriptor_manager.hpp"

#include "aii/array.hpp"

namespace Fs::Merofs{

// File Descriptor Table Chunks

FileDescriptorTable::TableChunk::TableChunk()
  :
    prev{nullptr},
    next{nullptr},
    used{0},
    table{}
{

}

FileDescriptorTable::TableChunk::~TableChunk()
{
}

std::uint8_t FileDescriptorTable::TableChunk::NextFree() const{
  for(std::uint8_t i = 0; i < table.Size(); i++){
    if(table[i].IsNull()){
      return i;
    }
  }
  return 0;
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

auto FileDescriptorTable::AllocateChunk() -> TableChunk*{
  auto allocchunk = Mem::Heap::Allocator::New<TableChunk>();
  TableChunk* parent = m_root;
  while(parent){
    if(parent->next){
      parent = parent->next;
    }
    else{
      parent->next = allocchunk;
    }
  }
  return allocchunk;
}

const FileDescriptor* FileDescriptorTable::Get(std::uint8_t fd) const{
  TableChunk* tableIndexer = m_root;
  if(fd > TableEntries()){
    tableIndexer = tableIndexer->next;
    fd -= TableEntries();
  }
  return &tableIndexer->Entry(fd);
}

FileDescriptor* FileDescriptorTable::Allocate(){
  TableChunk* tableIndexer = m_root;
  // first iterate through the existing table chunks to find a free fd
  while(tableIndexer){
    if(tableIndexer->Full()){
      tableIndexer = tableIndexer->next;
      continue;
    }
    else{
      // we have a free table chunk
      std::uint8_t freeindex = tableIndexer->NextFree();
      tableIndexer->Entry(freeindex) = FileDescriptor{freeindex};
      return &tableIndexer->Entry(freeindex);
    }
  }
  // no existing table was free, so allocate a new chunk
  auto* newchunk = AllocateChunk();
  std::uint8_t freeindex = newchunk->NextFree();
  newchunk->Entry(freeindex) = FileDescriptor(freeindex);
  return &newchunk->Entry(freeindex);
}

void FileDescriptorTable::Remove(std::uint8_t fd){
  TableChunk* tableIndexer = m_root;
  if(fd > TableEntries()){
    tableIndexer = tableIndexer->next;
    fd -= TableEntries();
  }
  tableIndexer->Entry(fd).Reset();
}

std::size_t FileDescriptorTable::NumberOfChunks() const{
  TableChunk* tableIndexer = m_root;
  std::size_t count = 0;
  if(!tableIndexer){
    return count;
  }
  while(tableIndexer->next){
    tableIndexer = tableIndexer->next;
    count++;
  }
  return count;
}

}
