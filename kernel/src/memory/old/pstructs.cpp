/*

#include "pstructs.hpp"

namespace memory::paging{
  PagingEntryDescriptor& PagingEntryDescriptor::operator=(
      const PagingEntryDescriptor& src
  ){
    m_entry = src.m_entry;
    return *this;
  }

  paddr64_t GetCurrentPM4L(){
    std::uint64_t currentCr3 = load_paging_table();
    paddr64_t pm4laddr = (currentCr3 >> 12) << 12;
    return pm4laddr;
  }

  void ParsePageTableCommon(PagingEntryDescriptor::EntryType ET, paddr64_t ptaddr){
    // universal function to parse a page table
    for(std::uint16_t j = 0; j < structureSize; j++){
      const PagingEntryDescriptor* const pe = 
        reinterpret_cast<P4eDescriptor*>(ptaddr + 8*j);
      switch(ET){
        using enum PagingEntryDescriptor::EntryType;
        case P4e:
          kout << "pml4e entry :: " << j;
          break;
        case Pdpte:
          kout << "Pdpte entry :: " << j;
          break;
        case Pde:
          kout << "Pde entry :: " << j;
          break;
        case Pte:
          kout << "Pte entry :: " << j;
          break;
      }
      kout << " :: value :: " << static_cast<std::uint64_t>(*pe) << '\n';
      if(pe->Present()){
        kout << "present" << '\n';
      }
      if(pe->Writeable()){
        kout << "writeable" << '\n';
      }
    }
  }

}

*/
