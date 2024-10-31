#include "memory/memory.hpp"
#include "memory/pages.hpp"

namespace integ::mem{

  inline void FindFaultingLinear(){
    // just a procedure for me to find a linear address which causes a 
    // page fault
    for(int i = 0; i < 0x1fe; i++){
      for(int l = 0; l < 0x1fe; l++){
        for(int j = 0; j < 0x1fe; j++){
          memory::vaddr64_t vaddr2 = 
            memory::FormLinearAddress(0x100, l, i, j, 0x000);
          char *ch2 = reinterpret_cast<char*>(vaddr2);
          kout << "vaddr2 :: " << vaddr2 << '\n';
          *ch2 = 'D';
          kout << "write successfully: " << *ch2 << '\n';
        }
      }
    }
    // note the bootloader paging table gives me a faulting address at
    // memory::FormLinearAddress(0x100, 0xA, 0x002, 0x002, 0x000);
  }

  inline void CheckPageFaultHandler(){
    // this procedure checks if the virtual address indeed page faults
    // ... it appears the pdpte with index A is not present at boot.
    memory::vaddr64_t vaddr2 = 
    memory::FormLinearAddress(0x100, 0xA, 0x002, 0x002, 0x000);
    char *ch2 = reinterpret_cast<char*>(vaddr2);
    kout << "vaddr2 :: " << vaddr2 << '\n';
    *ch2 = 'D'; // it indeed pagefaults :D
  }

  inline void WalkPageTable(){
    // this is for me to see how the boot page tables are set up
    memory::paging::ParsePM4L(memory::paging::GetCurrentPM4L());
    // pm4l entry ... references pdpt at 
    kout << "----\n";
    memory::paging::ParsePDPT(0x7FF25000);
    // pdpt entry ... references pd at 
    kout << "----\n";
    memory::paging::ParsePD(0x7FF23000);
    // pd entry ... references pt at 
    kout << "----\n";
    memory::paging::ParsePT(0x7FC00000);

  }

  inline void TestPagingIndicies(){
    // the following args should give me linear address [INSERT LINEAR]
    memory::vaddr64_t vaddr = 
      memory::FormLinearAddress(0x100, 0x001, 0x1FE, 0x002, 0x001);
    // now reversing the operation, it should give me the paging indices:
    // PM4L  index:
    // PDPT  index:
    // PD    index:
    // PT    index:
    // Page offset:
    memory::ExtractPagingIndices(vaddr);
  }

  inline void TestVirtualAddressToPage(){
    //kout << memory::VirtualToPageBase(vaddr);
  }

  inline void CheckTopOfRam(){
    // checking the virtual address tables for the top of ram
    // vaddr :: FFFF800280000000
    // page offset :: 0
    // pagetable index :: 0
    // page directory index :: 0
    // page directory pointer index :: A - NOT PRESENT
    // pm4l index :: 100 - present 7FF15027
    memory::paging::ParsePM4L(memory::paging::GetCurrentPM4L());
    kout << "----\n";
    memory::paging::ParsePDPT(0x7FF15000);
    /*
    kout << "----\n";
    memory::paging::ParsePD(0x7FF23000);
    kout << "----\n";
    memory::paging::ParsePT(0x7FC00000);
    */
  }

  inline void IDK(){
    // this was the last procedure in main.cpp ...
    // ill come back and understand what i was doing later ... 
    //memory::PrintSizeofPagetorMap();

    //memory::paging::ParseBootLoaderTable();

    //x8664::features::probePhysicalWidth();

    kout << intmode::hex;

    //x8664::interrupts::invokeInterrupt(5);

    // FFFF800280000000 is a faulting address
    // FFFF800280000000 is a faulting address
    //memory::ExtractPagingIndices(0xFFFF800280000000);
    // now we check why is it faulting by checking table entries
    /*
    kout << "----\n";
    memory::paging::ParsePDPT(0x7FF25000);
    kout << "----\n";
    memory::paging::ParsePD(0x7FF23000);
    kout << "----\n";
    memory::paging::ParsePT(0x7FC00000);
    */

    //memory::paging::ParsePage(0x53B0B3A0E031000);
    /*
    kout << intmode::hex;
    kout << "start of ram " << memory::startOfRam << '\n';
    memory::paddr64_t paddr = 0x4f901;
    kout << "physical memory location :: " << paddr << " :: page frame number :: " << memory::PhysicalToPageN(paddr) << '\n';
    */

    //memory::PrintPageFrames();

    /*
    kout << "attempting to read from pci device: " << io::pci::ReadDeviceHeader() << '\n';
    io::pci::EnumeratePCI();
    //Disk::InitialiseAtaPio(0, 0);
    */
  }

} // namespace integ
