#pragma once

#include <cstdint>

#include "ahci_port.hpp"

#include "drivers/ahci/structs/command_list.hpp"
#include "drivers/ahci/structs/recieved_fis.hpp"

#include "aii/array.hpp"

#include "kmalloc.hpp"
#include "kassert.hpp"

#include "drivers/pci/constants.hpp"
#include "drivers/pci/pci.hpp"
#include "drivers/serial/kostream.hpp"
#include "memory/address.hpp"

namespace Drivers::Ahci{

struct AhciDriver{
  struct [[gnu::packed]] HostRegisters{
    std::uint32_t cap;    // host capabilities
    std::uint32_t ghc;    // global host control
    std::uint32_t is;     // interrupt status
    std::uint32_t pi;     // ports implemented
  }; static_assert(sizeof(HostRegisters) == 4*4);

  constexpr static std::uint32_t MAX_PORTS = 32;

  public:
    AhciDriver();

    bool Present() const { return m_present;}
    bool& AhciEnabled(){ return m_enabled;}
    std::uint64_t Abar(){ return m_abar;}
    std::uint64_t MMIOBase(){ return m_mmioBase;}
    AhciPort& Port(std::uint32_t index){ return m_ports[index];}

    // host registers
    std::uint32_t Capabilites(){ return m_hostregs->cap;}
    void SetCapabilites(std::uint32_t val){ m_hostregs->cap = val;}
    std::uint32_t Ghc(){ return m_hostregs->ghc;}
    void SetGhc(std::uint32_t val){m_hostregs->ghc = val;}
    std::uint32_t Is(){ return m_hostregs->is;}
    void ClearIs(std::uint32_t index);
    std::uint32_t PortsImplemented(){ return m_hostregs->pi;}

    void Enable();
    void Disable();

    void ResetController();

    bool HbaIdle();
    void ForceHbaIdle();
    void EnumerateImplementedPorts();

  private:
    void Initialise();
    void InitPresent();

    void InitAbar();
    void InitMMIOBase();
    void SetHostRegisters(HostRegisters* base);
    void InitPorts();
    void InitMemory();

  private:
    bool m_present;
    bool m_enabled;
    std::uint64_t m_abar;
    std::uint64_t m_mmioBase;
    HostRegisters* m_hostregs;
    Aii::Array<AhciPort, 32> m_ports;
};

}
