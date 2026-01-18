#pragma once

// Class abstraction for the Local APIC Controller
//
// I wana make this a singleton
//
// For now, disable x2Apic mode.

#include <cstdint>

#include "x86_64/features/features.hpp"
#include "x86_64/features/msr.hpp"
//#include "memory/address.hpp"
#include "kassert.hpp"
#include "types.hpp"

namespace X8664::Interrupts{

class ApicController{

static constexpr std::uint32_t LVT_CMCI_OFFSET = 0x2f0;
static constexpr std::uint32_t LVT_TIMER_OFFSET = 0x320;
static constexpr std::uint32_t LVT_THERMAL_OFFSET = 0x330;
static constexpr std::uint32_t LVT_PERF_MON_OFFSET = 0x340;
static constexpr std::uint32_t LVT_LINT0_OFFSET = 0x350;
static constexpr std::uint32_t LVT_LINT1_OFFSET = 0x360;
static constexpr std::uint32_t LVT_ERROR_OFFSET = 0x370;

static constexpr std::uint32_t TIMER_INITIAL_OFFSET = 0x380;
static constexpr std::uint32_t TIMER_CURRENT_OFFSET = 0x390;
static constexpr std::uint32_t TIMER_DIVIDE_OFFSET = 0x3e0;

static constexpr std::uint8_t SPURIOUS_INTERRUPT_VECTOR = 0xff;

  public:
    ApicController();
    ApicController(const ApicController& src) = delete;
    ApicController(const ApicController&& src) = delete;

    void EnableController();
    void DisableController();

    void Enumerate();
    bool IsPresent();
    bool Supportsx2Apic();
    bool SupportsEoiSupression();

    void WriteEoi(std::uint32_t val);
    void SignalEoi();

    void BlockFlatInterrupts();
    void EnableFlatInterrupts();
    void SetTaskPriority(std::uint8_t);

    void SetSpuriousVector(std::uint8_t vector);

    // Registers
    
    Chisaka::PhysAddr64 GetBaseAddress() const;

    std::uint32_t ReadId() const;

    std::uint32_t ReadVersion() const;
    void EnumerateVersion() const;

    std::uint32_t ReadTpr() const;
    void WriteTpr(std::uint32_t val);

    std::uint32_t ReadPpr() const;

    std::uint32_t ReadDfr() const;

    std::uint32_t ReadSpuriousVectorRegister() const;
    void WriteSpuriousVectorRegister(std::uint32_t val);

    // Local Vector Table

  private:
    void InitialiseLvt();

  public:
    void EnumerateLvt() const;
    void EnumerateLvtEntry(std::uint32_t lvtEntry) const;

    std::uint32_t ReadLint0() const;
    void WriteLint0(std::uint32_t val);
    void MaskLint0();

    std::uint32_t ReadLint1() const;
    void WriteLint1(std::uint32_t val);

    std::uint32_t ReadError() const;
    void WriteError(std::uint32_t val);

    // Timer - technically part of the lvt

    void EnumerateTimer() const;

  private:
    Chisaka::PhysAddr64 m_base;
    bool m_enabled;
    bool m_present;
    bool m_x2apicSupport;
};

}
