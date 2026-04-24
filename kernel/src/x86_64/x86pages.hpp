#pragma once

// class to manage x86 page tables

#include <cstdint>

#include "paging/upper_page_table.hpp"
#include "paging/upper_page_entry.hpp"
#include "paging/page_table.hpp"
#include "paging/page_entry.hpp"

#include "kassert.hpp"

namespace X8664{
  class X86Pages{
    static constexpr std::uintptr_t CANONCIAL_MASK = 0xFFFF'8000'0000'0000;

    struct PageIndices{
      std::uint16_t pm4le;
      std::uint16_t pdpte;
      std::uint16_t pde;
      std::uint16_t pt;
    };

    public:
      static constexpr bool IsCanoncial(std::uintptr_t virtAddr){
        bool upperCanonical = (virtAddr & CANONCIAL_MASK) == CANONCIAL_MASK;
        bool lowerCanonical = (virtAddr & CANONCIAL_MASK) == 0;
        if(upperCanonical || lowerCanonical){
          return true;
        }
        else{
          return false; 
        }
      }

      static PageIndices ExtractPageIndicies(std::uintptr_t virtAddr);

    public:
      X86Pages(UpperPageTable* pml4base): m_pml4{pml4base}{}
      std::uintptr_t VirtToPhys(std::uintptr_t virtAddr);     // std::expected would be good here...
      UpperPageTable* Pml4(){ return m_pml4;}
      void PrintIndicies(std::uintptr_t address);

      void MapUserPage(std::uint64_t vaddr, std::uint64_t pagebase);
      void MapKernelTable(std::uintptr_t vkbase);

    private:
      UpperPageTable* m_pml4;
  };
}

