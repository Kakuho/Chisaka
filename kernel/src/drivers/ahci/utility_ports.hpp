#pragma once

#include "utility.hpp"
#include "structs/command_list.hpp"
#include "structs/recieved_fis.hpp"

#include "drivers/pci/constants.hpp"
#include "drivers/pci/pci.hpp"
#include "drivers/serial/kostream.hpp"

#include "kassert.hpp"
#include "aii/string.h"

namespace Drivers::Ahci::Ports{

using Pci::ioaddr32_t;


constexpr static std::uint32_t CMD_ST   = 0x0000'0001;
constexpr static std::uint32_t CMD_FRE  = 0x0000'0010;
constexpr static std::uint32_t CMD_FR   = 0x0000'4000;
constexpr static std::uint32_t CMD_CR   = 0x0000'8000;

//-------------------------------------------------------------
//  Port Registers
//-------------------------------------------------------------

std::uint32_t GetPortBaseAddress(std::uint8_t portno);
std::uint32_t* GetClbPtr(std::uint8_t portno);
std::uint32_t* GetClbuPtr(std::uint8_t portno);
std::uint32_t* GetFbPtr(std::uint8_t portno);
std::uint32_t* GetFbuPtr(std::uint8_t portno);
std::uint32_t* GetIsPtr(std::uint8_t portno);
std::uint32_t* GetIePtr(std::uint8_t portno);
std::uint32_t* GetCmdPtr(std::uint8_t portno);
std::uint32_t* GetSerrPtr(std::uint8_t portno);
std::uint32_t* GetCiPtr(std::uint8_t portno);

// Read Only
const std::uint32_t* GetTfdPtr(std::uint8_t portno);
const std::uint32_t* GetSstsPtr(std::uint8_t portno);

//-------------------------------------------------------------
//  Queries
//-------------------------------------------------------------

bool CheckImplementedPort(std::uint8_t port);

//-------------------------------------------------------------
// Functionalities
//-------------------------------------------------------------

bool PortIdle(std::uint8_t port);
void ForceIdle(std::uint8_t port);

void SetPortClb(std::uint8_t port, std::uint64_t addr);
void SetPortFb(std::uint8_t port, std::uint64_t addr);

void SetPortClb(std::uint8_t port, void* addr);
void SetPortFb(std::uint8_t port, void* addr) ;

void ClearPortSerr(std::uint8_t port);

bool PortHasDevicePresent(std::uint8_t port);

bool SetFre(std::uint8_t port);
bool ClearFre(std::uint8_t port);

bool SetFre(std::uint8_t port);
bool ClearFre(std::uint8_t port);

bool ClearInterruptStatus(std::uint8_t port);
bool EnableInterrupts(std::uint8_t port);

bool StartPortDMAEngine(std::uint8_t port);

void SetPortCommandIssue(std::uint8_t port, std::uint8_t slot);
bool PortCommandIssueStatus(std::uint8_t port, std::uint8_t slot);

bool CheckPortDHRS(std::uint8_t port);
bool CheckPortDPS(std::uint8_t port);

bool CheckPortTfdError(std::uint8_t port);
bool CheckPortTfdBsy(std::uint8_t port);
bool CheckPortTfdDrq(std::uint8_t port);
bool CheckPortTfdErr(std::uint8_t port);

//-------------------------------------------------------------
//  Port Structs 
//-------------------------------------------------------------

std::uint64_t GetCommandListBase(std::uint8_t port);
void PrintPortCommandListBase(std::uint8_t portno);
int FindEmptyCommandSlot(std::uint8_t port);
int FindEmptyCommandSlot(std::uint8_t port);

std::uint64_t GetReceivedBase(std::uint8_t port);

//-------------------------------------------------------------
//  Error Checking
//-------------------------------------------------------------

bool CheckAbortErrors(std::uint8_t portno);
bool CheckDataErrors(std::uint8_t portno);
bool CheckSystemMemoryErrors(std::uint8_t portno);

bool InterfaceErrorsPresent(std::uint8_t portno);

//-------------------------------------------------------------
//  Global Structs
//-------------------------------------------------------------

void PrintAllCommandListBase();

}
