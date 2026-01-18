#pragma once

#include "./../hpet_controller.hpp"

#include "firmware/acpi/hpet_table.hpp"
#include "firmware/acpi/acpi_manager.hpp"
#include "firmware/acpi/table_manager.hpp"

#include "x86_64/interrupts/interrupt_manager.hpp"
#include "x86_64/interrupts/controllers/pic.hpp"
#include "x86_64/segments/segment_manager.hpp"

// Code sample to poll main counter and timer comparators

namespace X8664::Timers::Samples::HpetPoll{
    static constexpr std::uint64_t MATCH_VALUE = 0x50000;

    inline void PollPeriodic(){
      // Polling the value of a periodic comparator
      Firmware::Acpi::TableManager acpiTables{};
      acpiTables.Initialise(static_cast<Firmware::Acpi::RsdpTable*>(Limine::rsdp_request.response->address));
      auto pHpetTable = acpiTables.HpetPtr();
      kout << "Hpet Base Address = " << pHpetTable->BaseAddress() << '\n';

      Timers::HpetController hpetController{};
      hpetController.SetBaseAddress(pHpetTable->BaseAddress());
      hpetController.Disable();
      hpetController.SetMainCounter(0x00);

      // Set up Timer 0 
      hpetController.EnumerateTimerCapabilities(0);
      hpetController.Timers()[0].SetEdgeTriggered();
      hpetController.Timers()[0].Set32BitMode();
      hpetController.Timers()[0].SetPeriodicMode();
      hpetController.Timers()[0].WriteMatchValue(MATCH_VALUE);
      hpetController.Timers()[0].WriteMatchValue(MATCH_VALUE);
      //hpetController.Timers()[0].Clear32BitMode();
      hpetController.Timers()[0].Enable();

      hpetController.Enable();
      while(true){
        volatile std::uint32_t low = hpetController.ReadMainCounterLow();
        volatile std::uint32_t high = hpetController.ReadMainCounterHigh();
        std::uint64_t counter = hpetController.ReadMainCounter();
        kout << "Counter: " << counter << ", high: " << high << ", low: " << low << '\n';

        volatile std::uint32_t comparator = hpetController.Timers()[0].GetMatchValue();
        kout << "Timer 0 Comparator Value: " << comparator << '\n';
      }
    }

  inline void PollMainCounter(){
    Firmware::Acpi::TableManager acpiTables{};
    acpiTables.Initialise(static_cast<Firmware::Acpi::RsdpTable*>(Limine::rsdp_request.response->address));
    auto pHpetTable = acpiTables.HpetPtr();
    kout << "Hpet Base Address = " << pHpetTable->BaseAddress() << '\n';
    Timers::HpetController hpetController{pHpetTable->BaseAddress()};

    hpetController.Disable();
    kout << "Hpet Disabled" << '\n';

    hpetController.SetMainCounter(0x00);
    kout << "main counter set to 0x00" << '\n';

    hpetController.Enable();

    while(true){
      volatile std::uint32_t low = hpetController.ReadMainCounterLow();
      volatile std::uint32_t high = hpetController.ReadMainCounterHigh();
      std::uint64_t counter = hpetController.ReadMainCounter();
      kout << "Counter: " << counter << ", high: " << high << ", low: " << low << '\n';
    }
  }

}
