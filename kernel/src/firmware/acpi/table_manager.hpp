#pragma once

#include "rsdp_table.hpp"
#include "rsdt_table.hpp"
#include "xsdt_table.hpp"

#include "kassert.hpp"

namespace Firmware::Acpi{
  class TableManager{
    public:
      TableManager() = default;
      void Initialise(RsdpTable* addr);

    private:
      RsdpTable* m_rsdp; 
      union{
        RsdtTable* m_rsdt;
        XsdtTable* m_xsdt;
      };
      bool isXsdt;
  };
}
