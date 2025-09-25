#pragma once

// A container to store pointers acpi tables.
// The pointers are all virtual pointers.

#include "rsdp_table.hpp"
#include "rsdt_table.hpp"
#include "xsdt_table.hpp"
#include "fadt_table.hpp"
#include "hpet_table.hpp"
#include "table_header.hpp"

#include "kassert.hpp"

namespace Firmware::Acpi{
  class TableManager{
    public:
      TableManager() = default;

      RsdtTable* RsdtPtr(){ return m_rsdt;}

      void Initialise(RsdpTable* addr);
      void InitialiseRoot(RsdpTable* addr);
      void InitialiseTablePtrs();

    private:
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
