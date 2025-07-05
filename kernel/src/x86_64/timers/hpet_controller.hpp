#pragma once

#include <cstdint>
#include <cassert>

#include "aii/array.hpp"
#include "kassert.hpp"
#include "./../../firmware/acpi/hpet_table.hpp"

namespace X8664::Timers::Hpet{

class Controller{

  struct TimerRegisters{
    void Enable() {config |=  0x04;}
    void Disable(){config &= ~0x04;}

    void WriteMatchValue(std::uint64_t val){comparator = val;}
    std::uint64_t GetMatchValue(){return comparator;}

    void SetPeriodicMode(){config |= 0x48;}
    void SetOneShotMode(){config &= ~0x8;}

    void Set32BitMode(){ config |= 0x100;}
    void Clear32BitMode(){ config &= ~0x100;}

    void SetLevelTriggered(){ config |= 2;}
    void SetEdgeTriggered(){ config &= ~2;}

    [[maybe_unused]] const volatile std::uint64_t rsv3;
    std::uint64_t config;
    std::uint64_t comparator;
    std::uint64_t fsbInterrupt;
  };

  struct Registers{
    const std::uint64_t gCapabilities;
    [[maybe_unused]] const std::uint64_t rsv0;

    std::uint64_t gConfig;
    [[maybe_unused]] const std::uint64_t rsv1;

    std::uint64_t gInterruptStatus;
    [[maybe_unused]] Aii::StaticArray<std::uint8_t, 0xc7> rsv2;

    std::uint32_t gMainCounterLo;
    std::uint32_t gMainCounterHi;

    Prim::StaticArray<TimerRegisters, 0> timers;
  };

  static_assert(sizeof(TimerRegisters) == 0x20);
  static_assert(sizeof(Registers) == 0xf8);

  public:
    explicit Controller();
    explicit Controller(Registers* base);
    explicit Controller(std::uint64_t base);

    void SetBaseAddress(Registers* base);
    void SetBaseAddress(std::uint64_t);

    void Enable() { m_regBase->gConfig |= 1;}
    void Disable(){ m_regBase->gConfig &= ~1;}

    Prim::StaticArray<TimerRegisters, 0>& Timers(){ return m_regBase->timers;}

    void EnumerateCapabilities() const;
    void EnumerateTimerCapabilities(std::uint8_t timerno) const;

    void ClearInterrupt(std::uint8_t timerno);
    constexpr std::uint64_t GetInterruptStatus() const 
    {return m_regBase->gInterruptStatus;}

    std::uint64_t ReadMainCounter();
    std::uint32_t ReadMainCounterLow(){ return m_regBase->gMainCounterLo;}
    std::uint32_t ReadMainCounterHigh(){ return m_regBase->gMainCounterHi;}
    void SetMainCounter(std::uint64_t val);

  private:
    void InitialiseTimer(std::uint8_t timerno);

    std::uint64_t CalculateFrequency() const;
    std::uint8_t CalculateNumTimers() const;

    constexpr bool LegacyRouteCapable() const{
      return ((m_regBase->gCapabilities & 0x8000) >> 15);
    }

    void SetLegacyReplacementRoute() const;

  private:
    Registers* m_regBase;
    std::uint32_t m_freq;
    std::uint8_t m_numTimers;
    bool m_running;
};

}
