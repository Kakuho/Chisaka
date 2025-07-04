#pragma once

#include "./../hpet_controller.hpp"

#include "firmware/acpi/hpet_table.hpp"
#include "firmware/acpi/acpi_manager.hpp"

#include "x86_64/interrupts/interrupt_manager.hpp"
#include "x86_64/interrupts/controllers/pic.hpp"
#include "x86_64/segments/segment_manager.hpp"

// Code sample to poll main counter and timer comparators

namespace X8664::Hpet::Samples::Poll{
    static constexpr std::uint64_t MATCH_VALUE = 0x50000;

    inline void PollPeriodic(){
      // Polling the value of a periodic comparator
      Firmware::Acpi::AcpiManager acpi{};
      Firmware::Acpi::HpetTable* pHpetTable = acpi.GetHpet();

      Timers::Hpet::Controller hpetController{};
      hpetController.SetBaseAddress(pHpetTable->GetHpetBaseAddress());
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
    Firmware::Acpi::AcpiManager acpi{};
    Firmware::Acpi::HpetTable* pHpetTable = acpi.GetHpet();
    Timers::Hpet::Controller hpetController{pHpetTable->GetHpetBaseAddress()};

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
