#pragma once

#include "structs/command_list.hpp"
#include "structs/recieved_fis.hpp"

#include "aii/array.hpp"
#include "kmalloc.hpp"

#include "drivers/pci/constants.hpp"
#include "drivers/pci/pci.hpp"
#include "drivers/serial/kostream.hpp"
#include "memory/address.hpp"

#include "kassert.hpp"

namespace Drivers::Ahci::Ports{
  //  Registers
  std::uint32_t* GetCmdPtr(std::uint8_t portno);

  bool PortIdle(std::uint8_t portno);
  void ForceIdle(std::uint8_t portno);

  void SetPortClb(std::uint8_t port, std::uint64_t addr);
  void SetPortClb(std::uint8_t port, void* addr);
  void SetPortFb(std::uint8_t port, std::uint64_t addr);
  void SetPortFb(std::uint8_t port, void* addr);

  void ClearPortSerr(std::uint8_t port);

  bool ClearInterruptStatus(std::uint8_t port);
  bool EnableInterrupts(std::uint8_t port);

  bool PortHasDevicePresent(std::uint8_t portno);
  bool StartPortDMAEngine(std::uint8_t portno);
}

namespace Drivers::Ahci{

static bool Reseted = false;

static bool AhciEnabled = false;

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

std::uint32_t operator&(std::uint32_t src, CAP_FLAGS flag);

using Pci::ioaddr32_t;

// These are older, I probabaly will change these code

void PrintModeSelect();

void CheckAhciMode();

void CheckMemoryAddressAccess();

//-------------------------------------------------------------
//  Important Address Values
//-------------------------------------------------------------

void PrintAbars();
std::uint64_t GetAbar();
std::uint64_t GetAhciBase();
std::uint32_t* GetCapPtr();
std::uint32_t* GetGhcPtr();
std::uint32_t* GetIsPtr();
const std::uint32_t* GetPiPtr();

//-------------------------------------------------------------
//  Initialisation Routine
//-------------------------------------------------------------

void InitialiseAhci();

void ResetController();

bool IsAhciController();

bool CheckFirmwareInitialisation();

void EnableAhci();

void EnumerateImplementedPorts();
Aii::Array<bool, 32> GetImplementedPorts();

bool IsHbaIdle();
void ForceHbaIdle();

std::uint8_t GetCommandSlots();

bool Supports64BitAddress();

void SetupMemory();  
void SetupMemory32();  
void SetupMemory64();  

void SetupMemory64FIXED();    // Fixed to a particular address for now,
                              // later would be better to assign using heap?

void EnablePortsFre();
void ClearAllPortSerr();

void ClearHostInterruptStatus();
void ClearPortsInterruptStatus();

void EnableHostInterrupts();
void EnablePortsInterrupts();

void EnableInterrupts();

//  For now I will do Polling IO, but later I will perform asynchronous 
//  IO, in which case the following needs to be implemented:
//   * Enable Interrupt for PCI
//   * Enable Interrupt for HBA
//   * Enable Interrupt for Every Port

void StartDMAEngines();

//-------------------------------------------------------------

void CheckDevicePresent();

//-------------------------------------------------------------

std::uint8_t NumberOfPorts();

void EnumerateCapabilities();

bool SupportsBiosHandoff();

void PrintBiosHandoff();

void PerformBiosHandoff();
}
