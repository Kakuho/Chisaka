#ifndef FIRMWARE_ACPI_ACPI_MANAGER_HPP
#define FIRMWARE_ACPI_ACPI_MANAGER_HPP

#include "tables.hpp"
#include "fadt.hpp"
#include "firmware/limine/requests.hpp"
#include "memory/address.hpp"

namespace Firmware::Acpi{

class AcpiManager{
  public:
    AcpiManager();

    // ------------------------------------------------------ //
    // Parsing
    // ------------------------------------------------------ //

    void ParseRsdt();
    void ParseXsdt();

  private:
    RsdpTable* m_rsdp;
    RsdtTable* m_rsdt;
    XsdtTable* m_xsdt;
    FadtTable* m_fadt;
};

}

#endif
