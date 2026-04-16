#include <print>
#include <cassert>

#include "page_fuzzer.hpp"
#include "memory_map_mock.hpp"
#include "integrity_check.hpp"

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
  MemmapMock::Get().InitFlat(1000 * 0x1000);
  std::cout << "Base: " << std::hex << MemmapMock::Get().Entry(0).base << '\n';
  std::cout << "Length: " << MemmapMock::Get().Entry(0).length << '\n';

  Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock> freelist;
  freelist.Init();

//  auto page = freelist.AllocatePage();
//  PrintPtr(page);
  std::cout << std::dec << (freelist.FreePagesLow() + freelist.FreePages()) << '\n';
}

//-----------------------------------------------------------------------------------------

void* FreeListAllocate(){
  using MemmapMock = Chisaka::Tests::MemoryMapMock;
  using PageAllocator = Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock>;
  return PageAllocator::Get().AllocatePage();
}

void FreeListDeallocate(void* base){
  using MemmapMock = Chisaka::Tests::MemoryMapMock;
  using PageAllocator = Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock>;

  PageAllocator::Get().DeallocPage(base);
}

bool FreeListExhausted(){
  using MemmapMock = Chisaka::Tests::MemoryMapMock;
  using PageAllocator = Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock>;

  return PageAllocator::Get().FreePagesLow() + PageAllocator::Get().FreePages() == 0;
}

bool FreeListSorted(){
  using MemmapMock = Chisaka::Tests::MemoryMapMock;
  using PageAllocator = Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock>;

  bool top = Chisaka::Tests::CheckSorted(PageAllocator::Get().Head());
  bool low = Chisaka::Tests::CheckSorted(PageAllocator::Get().LowHead());

  return top && low;
}

bool FreeListNoCycles(){
  using MemmapMock = Chisaka::Tests::MemoryMapMock;
  using PageAllocator = Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock>;

  bool top = Chisaka::Tests::CheckNoCycle(PageAllocator::Get().Head());
  bool low = Chisaka::Tests::CheckNoCycle(PageAllocator::Get().LowHead());

  return top && low;
}

bool FreeListNoDupes(){
  using MemmapMock = Chisaka::Tests::MemoryMapMock;
  using PageAllocator = Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock>;

  bool top = Chisaka::Tests::CheckNoDupes(PageAllocator::Get().Head());
  bool low = Chisaka::Tests::CheckNoDupes(PageAllocator::Get().LowHead());

  return top && low;
}


void RunFuzzyPageAlloc(){
  constexpr std::size_t PAGE_SIZE = 0x1000;
  using MemmapMock = Chisaka::Tests::MemoryMapMock;
  MemmapMock::Get().InitFlat(PAGE_SIZE * 100);

  std::cout << "Entry 0 Base: " << std::hex << MemmapMock::Get().Entry(0).base << '\n';
  std::cout << "Entry 0 Length: " << std::hex << MemmapMock::Get().Entry(0).length << '\n';
  std::cout << "Buffer Base: " << std::hex << MemmapMock::Get().BufferBase() << '\n';

  using PageAllocator = Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock>;
  PageAllocator::Get().Init();
  PageAllocator::Get().PrintFreeBasesLow();
  PageAllocator::Get().PrintFreeBases();

  Chisaka::Tests::PageFuzzer fuzzer{1, 0.3};
  fuzzer.SetAllocationFunction(FreeListAllocate);
  fuzzer.SetDeallocationFunction(FreeListDeallocate);
  fuzzer.SetExhaustedFunction(FreeListExhausted);
  
  fuzzer.AddIntegrityCheck(FreeListSorted, "Both Lists Sorted", "Unsorted");
  fuzzer.AddIntegrityCheck(FreeListNoCycles, "No cycles present", "a cycle is present");
  fuzzer.AddIntegrityCheck(FreeListNoDupes, "No dupes present", "a duplicate is present");

  fuzzer.Run(100000);
  fuzzer.DumpHistory("logs/test.log");
}

void RunFuzzyPageAllocDiscontigous(){
  constexpr std::size_t PAGE_SIZE = 0x1000;
  using MemmapMock = Chisaka::Tests::MemoryMapMock;
  MemmapMock::Get().InitDiscontigous(
    {
      {Chisaka::Tests::MemoryMapMock::Entry_t::Type::Useable,   3 * PAGE_SIZE},
      {Chisaka::Tests::MemoryMapMock::Entry_t::Type::Unuseable, 100 * PAGE_SIZE},
      {Chisaka::Tests::MemoryMapMock::Entry_t::Type::Useable,   4 * PAGE_SIZE},
    }
  );

  MemmapMock::Get().PrintDetails();

  using PageAllocator = Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock>;
  PageAllocator::Get().Init();
  PageAllocator::Get().PrintFreeBasesLow();
  PageAllocator::Get().PrintFreeBases();

  Chisaka::Tests::PageFuzzer fuzzer{1, 0.3};
  fuzzer.SetAllocationFunction(FreeListAllocate);
  fuzzer.SetDeallocationFunction(FreeListDeallocate);
  fuzzer.SetExhaustedFunction(FreeListExhausted);
  
  fuzzer.AddIntegrityCheck(FreeListSorted, "Both Lists Sorted", "Unsorted");
  fuzzer.AddIntegrityCheck(FreeListNoCycles, "No cycles present", "a cycle is present");
  fuzzer.AddIntegrityCheck(FreeListNoDupes, "No dupes present", "a duplicate is present");

  fuzzer.RunStrict();
  fuzzer.DumpHistory("logs/test.log");
}

void RunFuzzyPageAllocDefault(){
  using MemmapMock = Chisaka::Tests::MemoryMapMock;
  MemmapMock::Get().Init();

  //MemmapMock::Get().PrintDetails();

  using PageAllocator = Chisaka::PageAllocators::Freelist<Chisaka::Tests::MemoryMapMock>;
  PageAllocator::Get().Init();
  //PageAllocator::Get().PrintFreeBasesLow();
  //PageAllocator::Get().PrintFreeBases();

  Chisaka::Tests::PageFuzzer fuzzer{1, 0.3};
  fuzzer.SetAllocationFunction(FreeListAllocate);
  fuzzer.SetDeallocationFunction(FreeListDeallocate);
  fuzzer.SetExhaustedFunction(FreeListExhausted);
  
  fuzzer.AddIntegrityCheck(FreeListSorted, "Both Lists Sorted", "Unsorted");
  fuzzer.AddIntegrityCheck(FreeListNoCycles, "No cycles present", "a cycle is present");
  fuzzer.AddIntegrityCheck(FreeListNoDupes, "No dupes present", "a duplicate is present");

  fuzzer.RunStrict();
  fuzzer.DumpHistory("logs/test.log");
}


int main(){
  //RunFuzzyPageAlloc();
  //RunFreelist();
  //RunFuzzyPageAllocDiscontigous();
  RunFuzzyPageAllocDefault();
}
