#include "ptable.hpp"

namespace Chisaka::PTbl{

auto PTable::NextFreeId() const -> IdType{
  for(std::size_t i = 0; i < ENTRIES; i++){
    if(!Process(i).present){
      return i;
    }
  }
  return -1;
}

auto PTable::AllocateProcess(IdType parent) -> IdType{
  auto freeid = NextFreeId();
  m_procs[freeid].present = true;
  m_procs[freeid].parent_id = parent;
  return freeid;
}

void PTable::DeallocateProcess(IdType id){
  if(id > ENTRIES){
    return;
  }
  else{
    Process(id).present = false;
  }
}

}
