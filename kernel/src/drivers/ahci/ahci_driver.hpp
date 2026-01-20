#pragma once

#include <cstdint>

#include "ahci_port.hpp"
#include "ahci_disk.hpp"
#include "buffers/identify_device_buffer.hpp"

#include "drivers/ahci/structs/recieved_fis.hpp"
#include "drivers/ahci/structs/command_table.hpp"
#include "drivers/ahci/structs/command_list.hpp"
#include "drivers/ahci/structs/command_header.hpp"
#include "fis/h2d_register.hpp"
#include "fis/h2d_register.hpp"

#include "drivers/pci/constants.hpp"
#include "drivers/pci/pci.hpp"
#include "drivers/serial/kostream.hpp"

#include "kmalloc.hpp"
#include "kassert.hpp"

#include "kcontext.hpp"

#include "aii/array.hpp"

namespace Chisaka::Ahci{
  using namespace Chisaka;
struct AhciDiskPolled;

struct AhciDriver{
  struct [[gnu::packed]] HostRegisters{
    volatile std::uint32_t cap;    // host capabilities
    volatile std::uint32_t ghc;    // global host control
    volatile std::uint32_t is;     // interrupt status
    volatile std::uint32_t pi;     // ports implemented
  }; static_assert(sizeof(HostRegisters) == 4*4);

  constexpr static std::uint32_t MAX_PORTS = 32;

  // useful for troubleshooting the driver by giving fixed addresses
  constexpr static std::uint64_t COMMANDLIST_START = 0x100000000;
  constexpr static std::uint64_t FIS_START = 0x123000000;
  constexpr static std::uint64_t HBA_RAM_START = COMMANDLIST_START;
  constexpr static std::uint64_t HBA_RAM_END = FIS_START + 32*sizeof(RecievedFis);

  enum CAP_FLAGS: std::uint32_t{
    S64A = 0x8000'0000,
    SNCQ = 0x4000'0000,
    SSNTF = 0x2000'0000,
    SAM = 0x0001'0000,
    NCS = 0x1F00
  };

  public:
    AhciDriver();

    static AhciDriver& Get();
    void Init(){ Initialise();}
    bool Present() const { return m_present;}
    bool& AhciEnabled(){ return m_enabled;}
    std::uint64_t Abar() const{ return m_abar;}

    std::uint64_t MMIOBase() const{ return m_mmioBase;}
    AhciPort& Port(std::uint32_t index){ return m_ports[index];}
    Aii::Array<AhciPort, MAX_PORTS>& GetPorts()
    { return m_ports;}

    AhciDisk& Disk(std::uint8_t index){ return m_disks[index];}

    // host registers
    std::uint32_t Capabilites() const{ return m_hostregs->cap;}
    void SetCapabilites(std::uint32_t val){ m_hostregs->cap = val;}
    std::uint32_t Ghc() const{ return m_hostregs->ghc;}
    void SetGhc(std::uint32_t val){m_hostregs->ghc = val;}
    std::uint32_t Is(){ return m_hostregs->is;}
    void ClearIs();
    void ClearIs(std::uint32_t index);
    std::uint32_t PortsImplemented(){ return m_hostregs->pi;}

    void Enable();
    void Disable();
    void ResetController();

    void ClearHBAInterruptStatus();
    void EnableHBAInterrupts();
    void DisableHBAInterrupts();

    void ClearPortsInterruptStatus();
    void EnablePortsInterrupts();
    void DisablePortsInterrupts();

    void EnableInterrupts();
    
    std::uint32_t HBACommandSlots() const;

    bool HbaIdle();
    void ForceHbaIdle();
    void EnumerateImplementedPorts();

    void StartDMAEngines();

    void PrintPortDevicePresent();
    void PrintCapabilities();

    void WaitForPortInterrupt(std::uint8_t port);

    // Disk Operations
    void Perform(std::uint8_t port, CommandHeader& cmdheader, CommandTable* cmdtbl);

    void IdentifyDevice(std::uint8_t port, std::uint8_t* buffer);
    void IdentifyDevice(std::uint8_t port, IdentifyDeviceBuffer* buffer){
      IdentifyDevice(port, reinterpret_cast<std::uint8_t*>(buffer));
    }

    void WriteSector(std::uint8_t port, std::uint64_t sector, std::uint8_t* ibuffer);
    void WriteSector(std::uint8_t port, std::uint64_t sector, 
                    std::uint8_t* ibuffer, CommandTable* cmdtbl);

    void ReadSector(std::uint8_t port, std::uint64_t address, std::uint8_t* buffer);
    void ReadSector(std::uint8_t port, std::uint64_t address, 
                    std::uint8_t* buffer, CommandTable* cmdtable);

    void WriteSectorPolled(std::uint8_t port, std::uint64_t sector, 
        std::uint8_t* buffer);
    void WriteSectorPolled(std::uint8_t port, std::uint64_t sector, 
        std::uint8_t* buffer, CommandTable* cmdtable);

    void ReadSectorPolled(std::uint8_t port, std::uint64_t address, 
        std::uint8_t* buffer);
    void ReadSectorPolled(std::uint8_t port, std::uint64_t address, 
        std::uint8_t* buffer, CommandTable* cmdtable);


    /*
     * todo: read and write to multiple sectors
    void WriteSector(std::uint8_t port, std::uint64_t base, 
                     std::uint8_t count, std::uint8_t* buffer);
    void ReadSector(std::uint8_t port, std::uint64_t base, 
                    std::uint8_t count, std::uint8_t* buffer);
    */

    static AhciDisk CreatePolledDisk(std::uint8_t port);

  private:
    void Initialise();
    void InitPresent();

    void InitAbar();
    void InitMMIOBase();
    void SetHostRegisters(HostRegisters* hostregs){ m_hostregs = hostregs;}
    void InitPorts();
    void SetPortDevices();
    void InitMemory();

    void EnablePortsFRE();
    void DisablePortsFRE();
    void ClearPortsSERR();

    void InitDisks();
    void SetupDisk(std::uint8_t port);

  private:
    bool m_present;
    bool m_enabled;
    std::uint64_t m_abar;
    std::uint64_t m_mmioBase;
    volatile HostRegisters* m_hostregs;
    Aii::Array<AhciPort, MAX_PORTS> m_ports;
    Aii::Array<AhciDisk, MAX_PORTS> m_disks;
};

}
