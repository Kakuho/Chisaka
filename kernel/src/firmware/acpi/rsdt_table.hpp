#pragma once

//  Reference ACPI: 5.2.7
//  Root System Description Table (RSDT), Contains other acpi description tables. 
//  Must be ignored if XSDT is present in the RSDP table.

#include <cassert>

#include "table_header.hpp"
#include "memory/address.hpp"

#include "aii/array.hpp"
#include "drivers/serial/kostream.hpp"
#include "types.hpp"

#include "kcontext.hpp"

namespace Firmware::Acpi{
  struct [[gnu::packed]] RsdtTable{
    static constexpr Aii::Array<char, 4> SIG ={'R', 'S', 'D', 'T'};

    public:
      constexpr bool IsSignatureCorrect() noexcept{return header.signature == SIG;}


      TableHeader* EntryHeader(std::size_t index) const noexcept{
        //return reinterpret_cast<TableHeader*>(&tablePointers[index]);
      }

      TableHeader* VirtEntryHeader(std::size_t index) const noexcept{
        Chisaka::PhysAddr32 paddr = tablePointers[index];
        Chisaka::VirtAddr vaddr = Chisaka::KContext::Get().PhysToVirtAddr(paddr);
        return reinterpret_cast<TableHeader*>(vaddr);
      }

      void PrintTableLocations() const noexcept{
        for(std::size_t i = 0; i < header.length; i++){
          kout << "i = : " << i << '\n';
          kout << "Table Base: " << Chisaka::KContext::Get().PhysToVirtAddr(tablePointers[0]) << '\n';
        }
        kout << "fin" << '\n';
      }

      void PrintTableHeaders() const noexcept{
        for(std::size_t i = 0; i < header.length; i++){
          TableHeader* nextheader = VirtEntryHeader(i);
          if(nextheader){
            nextheader->PrintHeader();
            kout << '\n';
          }
          else{
            continue;
          }
        }
      }

    public:
      TableHeader header;
      Chisaka::PhysAddr32 tablePointers[1];
  };
  static_assert(sizeof(RsdtTable) == 40);
}
