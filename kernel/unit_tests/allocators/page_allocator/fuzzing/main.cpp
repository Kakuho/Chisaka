#include <print>
#include "page_fuzzer.hpp"
#include "memory_map_mock.hpp"
#include "ram/page_allocators/freelist.hpp"


int hi;

void* Allocate(){
  //return malloc(4);
  return &hi;
}

void Deallocate(void* base){
  //free(base);
}

void PrintPtr(void* ptr){
  std::println("0x{:0>16x}", reinterpret_cast<std::uintptr_t>(ptr));
}

void RunFuzzer(){
  Chisaka::Tests::PageFuzzer fuzzer{1, 0.5};
  fuzzer.SetAllocationFunction(Allocate);
  fuzzer.SetDeallocationFunction(Deallocate);
  fuzzer.Run(10);
  fuzzer.DumpHistory("test.log");
}

void RunMemMapMock(){
  auto memmap = Chisaka::Tests::MemoryMapMock::Get();
  memmap.InitFlat(16);
  memmap.DumpMemory("dump.log");
}

void RunFreelist(){
  using MemmapMock = Chisaka::Tests::MemoryMapMock;
  using PageAllocator = Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock>;
  MemmapMock::Get().InitFlat(6000);

  Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock> freelist;
  freelist.Init();
  auto page = freelist.AllocatePage();
  PrintPtr(page);
}

int main(){
  RunFreelist();
}
