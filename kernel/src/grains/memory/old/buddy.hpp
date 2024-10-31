#include "memory/pageframe.hpp"
#include "memory/pmm/buddy.hpp"
#include "helpers.hpp"
#include "serial/kostream.hpp"

namespace integ{
  inline void TestPhysicalMemoryDescriptor(){
    memory::pmm::buddy::PhysicalMemory pmtor{};
    pmtor.PrintPhysicalMap();
    // function to test the page chunk bookkeeping of memory
    memory::pmm::buddy::PrintFreeChunk(pmtor.FindChunks(0));
    kout << "-----------------\n";
    pmtor.PrintChunkDescriptors(2);
  }

  inline void TestMemoryBookeeping(){
    // procedure to test the bookeeping function of physical memory descriptor
    const memory::pmm::buddy::PhysicalMemory pmtor{};
    pmtor.PrintPhysicalMap();
    kout << "top of ram :: " << pmtor.GetTopOfPhysicalRam() << '\n';
    kout << "usable pages :: " << pmtor.GetUsuablePages() << '\n';
  }

  inline void TestUsableMemoryRanges(){
    integ::Begin();
    using namespace memory::pmm;
    const memory::pmm::buddy::PhysicalMemory pmtor{};
    for(std::uint8_t i = 0; i < buddy::PhysicalMemory::ranges; i++){
      const buddy::MemoryRange range = pmtor.GetRange(i);
      kout << "memory range from :: " << range.lower
           << " :: to :: " << range.higher << '\n';
      kout << "length :: " << range.Length()
           << " :: number of pages :: " << range.Pages() << '\n';
    }
    integ::End();
  }

  inline void TestPageFrameBases(){
    integ::Begin();
    using namespace memory;
    using namespace memory::pmm;
    const memory::pmm::buddy::PhysicalMemory pmtor{};
    PageFrames pageframes{pmtor};

    pageframes.PrintBases();
    integ::End();
  }

  inline void TestSizeofPageFramesArray(){
    integ::Begin();
    using namespace memory;
    using namespace memory::pmm;
    const memory::pmm::buddy::PhysicalMemory pmtor{};
    PageFrames pageframes{pmtor};
    kout << "size of a page frame descriptor :: " << sizeof(PageFrameDescriptor) 
         << '\n';
    kout << "total size of page frame array:: " << pageframes.Size(pmtor) << '\n';
    kout << "total pages available :: " << pmtor.GetUsuablePages() << '\n';
    integ::End();
  }

  /*
  pmtor.FindChunks(1);
  pmtor.FindChunks(2);
  pmtor.FindChunks(3);
  pmtor.FindChunks(4);
  */
  //pmtor.PrintChunkDescriptors(0);

}
