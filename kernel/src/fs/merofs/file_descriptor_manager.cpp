#include <cstdint>

#include"file_descriptor_manager.hpp"

#include "aii/array.hpp"

namespace Fs::Merofs{

FileDescriptorTable::TableChunk::TableChunk()
  :
    next{nullptr},
    prev{nullptr},
    table{}
{

}

FileDescriptorTable::TableChunk::~TableChunk()
{
}


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

const FileDescriptor* FileDescriptorTable::GetFd(std::uint8_t fd) const{
  
}

}
