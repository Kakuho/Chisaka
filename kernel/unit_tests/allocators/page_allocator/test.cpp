#include "page_fuzzer.hpp"

void* Allocate(){
  return nullptr;
}

void Deallocate(void* base){
}

int main(){
  Chisaka::Tests::PageFuzzer fuzzer;
  fuzzer.SetAllocationFunction(Allocate);
  fuzzer.SetDeallocationFunction(Allocate);
  fuzzer.Run(10);
}
