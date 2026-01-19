#pragma once

// A container to store pointers acpi tables.
// The pointers are all virtual pointers.

#include "tables/rsdp_table.hpp"
#include "tables/rsdt_table.hpp"
#include "tables/xsdt_table.hpp"
#include "tables/fadt_table.hpp"
#include "tables/hpet_table.hpp"
#include "tables/table_header.hpp"

#include "kassert.hpp"
#include "kcontext.hpp"

namespace Chisaka{
  class Acpi{
    public:
      static Acpi& Get(){ static Acpi a; return a;}
      void Init(RsdpTable* addr){ Initialise(addr);}

      RsdtTable* RsdtPtr(){ return m_rsdt;}
      HpetTable* HpetPtr(){ return m_hpet;}

    private:
      void Initialise(RsdpTable* addr);
      void InitialiseRoot(RsdpTable* addr);
      void InitialiseTablePtrs();

      RsdpTable* m_rsdp; 
      union{
        RsdtTable* m_rsdt;
        XsdtTable* m_xsdt;
      };
      bool isXsdt;
      FadtTable* m_fadt;
      HpetTable* m_hpet;
  };
}
