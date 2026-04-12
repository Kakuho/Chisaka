#include "page_fuzzer.hpp"
#include <cstddef>

namespace Chisaka::Tests{

void PageFuzzer::Run(std::size_t iterations){
  for(std::size_t i = 0; i < iterations; i++){
    FuzzAction action = GenAction();
    if(action == FuzzAction::Allocate){
      void* page = m_alloc();
      bool result = ValidateAllocated(page);
      AddToHistory(action, result);
    }
    else if(action == FuzzAction::Deallocate){
      void* page = PickFromAllocated();
      bool result = ValidateDeallocated(page);
      m_dealloc(page);
      AddToHistory(action, result);
      m_allocated.erase(m_allocated.find(page));
    }
  }
}

bool PageFuzzer::ValidateAllocated(void* page){
  if(m_allocated.contains(page)){
    return false;
  }
  return true;
}

bool PageFuzzer::ValidateDeallocated(void* page){
  if(!m_allocated.contains(page)){
    return false;
  }
  return true;
}

FuzzAction PageFuzzer::GenAction() const{
}

}
