#pragma once

#include "rsdp_table.hpp"
#include "fadt_table.hpp"
#include "rsdt_table.hpp"
#include "xsdt_table.hpp"

#include "limine/utility.hpp"

namespace Firmware::Acpi{

class AcpiManager{
  public:
    AcpiManager();
    void ParseRsdt();
    void ParseXsdt();

  private:
    RsdpTable* m_rsdp;
    RsdtTable* m_rsdt;
    XsdtTable* m_xsdt;
    FadtTable* m_fadt;
};

}
