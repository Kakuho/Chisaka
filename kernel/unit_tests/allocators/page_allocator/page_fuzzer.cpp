#include "page_fuzzer.hpp"
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <random>
#include <unordered_set>

namespace Chisaka::Tests{

PageFuzzer::PageFuzzer(std::size_t seed, float balance)
  :
    m_alloc{},
    m_dealloc{},
    m_actionGenerator{seed, balance},
    m_history{},
    m_allocated{}
{

}

void PageFuzzer::Run(std::size_t iterations){
  for(std::size_t i = 0; i < iterations; i++){
    FuzzAction action = GenAction();
    if(action == FuzzAction::Allocate){
      void* page = m_alloc();
      ValidationResult result = ValidateAllocated(page);
      AddToHistory(action, page, result);
      m_allocated.insert(page);
    }
    else if(action == FuzzAction::Deallocate){
      void* page = PickFromAllocated();
      ValidationResult result = ValidateDeallocated(page);
      m_dealloc(page);
      AddToHistory(action, page, result);
      m_allocated.erase(m_allocated.find(page));
    }
  }
}

auto PageFuzzer::ValidateAllocated(void* page) -> ValidationResult{
  if(m_allocated.contains(page)){
    return ValidationResult::Failure(0);
  }
  return ValidationResult::Success();
}

auto PageFuzzer::ValidateDeallocated(void* page) -> ValidationResult{
  if(!m_allocated.contains(page)){
    return ValidationResult::Failure(0);
  }
  return ValidationResult::Success();
}

FuzzAction PageFuzzer::GenAction() const{
  if(m_allocated.empty()){
    return FuzzAction::Allocate;
  }
  return m_actionGenerator.Gen();
}

void* PageFuzzer::PickFromAllocated() const{
  if(m_allocated.size() == 1){
    auto it = m_allocated.begin();
    return *it;
  }
  else{
    FuzzIndexPicker<std::mt19937> picker{m_actionGenerator.gen, m_allocated.size()};
    std::size_t index = picker.Gen();
    auto it = m_allocated.begin();
    std::advance(it, index);
    return *it;
  }
}

void PageFuzzer::AddToHistory(FuzzAction action, void* page, ValidationResult result){
  m_history.emplace(action, page, result);
}

void PageFuzzer::DumpHistory(const std::string& file_name){
  std::ofstream ofst{file_name};
  while(!m_history.empty()){
    FuzzResult result = m_history.front();
    ofst << std::format("{:>10} 0x{:0>16x} {:>8}", 
                        ToString(result.action), 
                        reinterpret_cast<std::uintptr_t>(result.address),
                        result.result.result ? "Success!" : "Fail!") 
         << '\n';
    if(result.result.result == false){

    }
    m_history.pop();
  }
}

}
