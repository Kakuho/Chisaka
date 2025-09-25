#pragma once

#include "firmware/acpi/table_manager.hpp"

namespace Firmware::Acpi::Samples{
  inline void QueryTableHeaders(){
    Firmware::Acpi::TableManager manager;
    manager.Initialise(static_cast<Firmware::Acpi::RsdpTable*>(limine::requests::rsdp_request.response->address));
    auto prsdt = manager.RsdtPtr();
    //prsdt->PrintTableLocations();
    prsdt->PrintTableHeaders();

    auto hpetptr = manager.HpetPtr();
    kassert(hpetptr->IsSignatureCorrect());

    hpetptr->gasAddr.PrintValues();
  }
}
