#include "page_fuzzer.hpp"

int hi;

void* Allocate(){
  //return malloc(4);
  return &hi;
}

void Deallocate(void* base){
  //free(base);
}

int main(){
  Chisaka::Tests::PageFuzzer fuzzer{1, 0.5};
  fuzzer.SetAllocationFunction(Allocate);
  fuzzer.SetDeallocationFunction(Deallocate);
  fuzzer.Run(10);
  fuzzer.DumpHistory("test.log");
}
