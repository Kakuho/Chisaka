#include "pageframe.hpp"
#include "buddy.hpp"

namespace memory{

  PageFrames::PageFrames(const pmm::buddy::PhysicalMemory& pmmDescriptor): 
    m_npf{pmmDescriptor.GetUsuablePages()}{
      //Initialise(pmmDescriptor);
  }

  void PageFrames::Initialise(const pmm::buddy::PhysicalMemory& pmmDescriptor){
    // we may require page frame boundary alignment
    m_buffer = reinterpret_cast<PageFrameDescriptor*>
              (
               paddrToVaddr(pmmDescriptor.GetTopOfPhysicalRam()) -
               pmmDescriptor.GetUsuablePages() * sizeof(PageFrameDescriptor)
              );
    SetPageFrameBase(pmmDescriptor);
  }

  void PageFrames::SetPageFrameBase(const pmm::buddy::PhysicalMemory& pmmDescriptor)
  {
    std::uint64_t pageOffset = 0;
    for(int i = 0; i < pmmDescriptor.ranges; i++){
      const pmm::buddy::MemoryRange currentMemRange = pmmDescriptor.GetRange(i);
      if(currentMemRange.IsEmpty()){
        continue;
      }
      paddr64_t currentBase = currentMemRange.lower;
      if(i != 0){
        pageOffset += currentMemRange.Pages();
      }
      for(std::uint64_t j = 0; j < currentMemRange.Pages(); j++){
        m_buffer[j + pageOffset].base = currentBase;
        currentBase += 0x1000;
      }
    }
  }

  std::uint64_t PageFrames::Size(const pmm::buddy::PhysicalMemory& pmmdtor) const{
    return pmmdtor.GetUsuablePages() * sizeof(PageFrameDescriptor);
  }

  void PageFrames::PrintBases() const{
    PrintBases(m_npf);
  }

  void PageFrames::PrintBases(std::uint64_t limit) const{
    for(std::uint64_t i = 0; i < limit; i++){
      kout << "Page frame :: i :: " << i << " :: has base :: " << m_buffer[i].base << '\n';
    }
  }
}
