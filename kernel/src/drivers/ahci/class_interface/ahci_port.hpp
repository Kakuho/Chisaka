#pragma once

#include <cstdint>

#include "drivers/serial/kostream.hpp"

namespace Drivers::Ahci{

struct AhciPort{
  struct PortRegisters{
    volatile std::uint32_t clistb;
    volatile std::uint32_t clistbu;
    volatile std::uint32_t fisb;
    volatile std::uint32_t fisbu;
    volatile std::uint32_t intStat;
    volatile std::uint32_t intEnable;
    volatile std::uint32_t cmd;
  };

  // useful constants for the command status registers
  constexpr static std::uint32_t CMD_ST   = 0x0000'0001;
  constexpr static std::uint32_t CMD_FRE  = 0x0000'0010;
  constexpr static std::uint32_t CMD_FR   = 0x0000'4000;
  constexpr static std::uint32_t CMD_CR   = 0x0000'8000;

  public:
    AhciPort();

    void SetMMIOBase(PortRegisters* base){ m_registers = base;}
    void SetPresent(){m_present = true;}
    bool Present() const{return m_present;}

    // registers
    void SetCLB(std::uint32_t addr){ m_registers->clistb = addr;}
    void SetCLB(std::uint64_t addr){ m_registers->clistb = addr;}
    void SetFisB(std::uint32_t addr){ m_registers->fisb = addr;}
    void SetFisB(std::uint64_t addr){ m_registers->fisb = addr;}
    volatile std::uint32_t& CommandStatus(){ return m_registers->cmd;}
  
    bool Idle();

    void ForceIdle();

  private:


  private:
    PortRegisters* m_registers;
    bool m_present;
};

}
