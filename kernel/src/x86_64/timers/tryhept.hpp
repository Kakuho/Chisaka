#ifndef X8664_TRYHPET_HPP
#define X8664_TRYHPET_HPP

#include "hpet_controller.hpp"
#include "x86_64/interrupts/controllers/pic.hpp"
#include "firmware/acpi/hpet_table.hpp"
#include "firmware/acpi/acpi_manager.hpp"

#include "x86_64/interrupts/interrupt_manager.hpp"
#include "x86_64/segments/segment_manager.hpp"

extern "C"{
  void enable_interrupts();
}

namespace X8664::TryHpet{
  static constexpr std::uint64_t MATCH_VALUE = 0x50000;

  Timers::Hpet::Controller hpetController;
  Interrupts::PicController picController{0x20};

  void InterruptHandler_Period(){
    //hpetController.Disable();
    kout << "Timer Interrupted!" << '\n';
    kout << "Interrupt Status: " << hpetController.GetInterruptStatus() << '\n';
    hpetController.ClearInterrupt(0);
    picController.SignalEOI(0);
    //hpetController.Enable();
  }

  //[[gnu::interrupt]]
  void InterruptHandler_OneShot(){
    hpetController.Disable();
    kout << "Timer Interrupted!" << '\n';
    kout << "Interrupt Status: " << hpetController.GetInterruptStatus() << '\n';
    kout << "Setting Controller Count to zero" << '\n';
    kout << hpetController.ReadMainCounter() << '\n';
    while(hpetController.GetInterruptStatus()){
      hpetController.ClearInterrupt(0);
    }
    while(picController.GetIsr()){
      picController.SignalEOI(0);
    }
    while(picController.GetIrr()){
      picController.SignalEOI(0);
    }
    hpetController.SetMainCounter(0x00);
    while(hpetController.ReadMainCounter()){
      ;;
    }
    hpetController.Timers()[0].WriteMatchValue(MATCH_VALUE);
    hpetController.Timers()[0].Enable();
    hpetController.Enable();
  }

  void TryPeriod(){
    // Set up GDT and IDT because we wana interrupt
    SegmentManager segManager{};
    segManager.LoadCurrent();
    segManager.ReloadSegmentRegisters();
    InterruptManager intManager{};
    intManager.EnableInterrupts();
    intManager.InstallISR(0x20, InterruptHandler_Period);
    //Interrupts::PicController picController{0x20};
    Firmware::Acpi::AcpiManager acpi{};
    Firmware::Acpi::HpetTable* pHpetTable = acpi.GetHpet();
    kout << "Hpet Base Address = " << pHpetTable->GetHpetBaseAddress() << '\n';
    hpetController.SetBaseAddress(pHpetTable->GetHpetBaseAddress());
    hpetController.EnumerateCapabilities();
    hpetController.Disable();
    hpetController.SetMainCounter(0x00);
    //-------------------------------------------------------------
    // Set the timer
    hpetController.EnumerateTimerCapabilities(0);
    hpetController.Timers()[0].SetEdgeTriggered();
    hpetController.Timers()[0].Set32BitMode();
    hpetController.Timers()[0].SetPeriodicMode();
    hpetController.Timers()[0].WriteMatchValue(MATCH_VALUE);
    hpetController.Timers()[0].WriteMatchValue(MATCH_VALUE);
    //hpetController.Timers()[0].Clear32BitMode();
    hpetController.Timers()[0].Enable();
    kout << '\n';
    hpetController.EnumerateTimerCapabilities(0);
    //-------------------------------------------------------------
    // Set the Counter and enable
    hpetController.Enable();
    while(true){
      volatile std::uint32_t low = hpetController.ReadMainCounterLow();
      volatile std::uint32_t high = hpetController.ReadMainCounterHigh();
      std::uint64_t counter = hpetController.ReadMainCounter();
      kout << "Counter: " << counter << ", high: " << high << ", low: " << low << '\n';

      volatile std::uint32_t comparator = hpetController.Timers()[0].GetMatchValue();
      kout << "Timer Comparator Value: " << comparator << '\n';
    }
  }

  void TryOneShot(){
    // Set up GDT and IDT because we wana interrupt
    SegmentManager segManager{};
    segManager.LoadCurrent();
    segManager.ReloadSegmentRegisters();
    InterruptManager intManager{};
    intManager.EnableInterrupts();
    intManager.InstallISR(32, InterruptHandler_OneShot);
    //Interrupts::PicController picController{32};
    Firmware::Acpi::AcpiManager acpi{};
    Firmware::Acpi::HpetTable* pHpetTable = acpi.GetHpet();
    kout << "Hpet Base Address = " << pHpetTable->GetHpetBaseAddress() << '\n';
    hpetController.SetBaseAddress(pHpetTable->GetHpetBaseAddress());
    hpetController.EnumerateCapabilities();
    hpetController.Disable();
    hpetController.SetMainCounter(0x00);
    //-------------------------------------------------------------
    // Set the timer
    hpetController.EnumerateTimerCapabilities(0);
    hpetController.Timers()[0].SetLevelTriggered();
    hpetController.Timers()[0].Set32BitMode();
    hpetController.Timers()[0].SetOneShotMode();
    hpetController.Timers()[0].WriteMatchValue(MATCH_VALUE);
    hpetController.Timers()[0].Enable();
    kout << '\n';
    hpetController.EnumerateTimerCapabilities(0);
    //-------------------------------------------------------------
    // Set the Counter and enable
    hpetController.Enable();
    while(true){
      volatile std::uint32_t low = hpetController.ReadMainCounterLow();
      volatile std::uint32_t high = hpetController.ReadMainCounterHigh();
      std::uint64_t counter = hpetController.ReadMainCounter();
      kout << "Counter: " << counter << ", high: " << high << ", low: " << low << '\n';
    }
  }
}

#endif
