#pragma once

#include <cstdint>

#include "drivers/serial/kostream.hpp"
#include "drivers/ahci/structs/command_list.hpp"

namespace Drivers::Ahci{

struct AhciPort{
  struct PortRegisters{
    volatile std::uint32_t clistb;
    volatile std::uint32_t clistbu;
    volatile std::uint32_t fisb;
    volatile std::uint32_t fisbu;
    volatile std::uint32_t is;
    volatile std::uint32_t ie;
    volatile std::uint32_t cmd;
    volatile std::uint32_t rsv;
    volatile std::uint32_t tfd;
    volatile std::uint32_t sig;
    volatile std::uint32_t ssts;
    volatile std::uint32_t sctl;
    volatile std::uint32_t serr;
    volatile std::uint32_t sact;
    volatile std::uint32_t ci;
    volatile std::uint32_t sntf;
    volatile std::uint32_t fbs;
  };

  // useful constants for the command status registers
  constexpr static std::uint32_t CMD_ST   = 0x0000'0001;
  constexpr static std::uint32_t CMD_FRE  = 0x0000'0010;
  constexpr static std::uint32_t CMD_FR   = 0x0000'4000;
  constexpr static std::uint32_t CMD_CR   = 0x0000'8000;

  // useful constants for interrupt enable register
  constexpr static std::uint32_t ENABLE_ALL_INTS = 0xfff0'00ff;

  // PxSTS
  static constexpr std::uint32_t TFD_STATUS     = 0xFF;
  static constexpr std::uint32_t TFD_STS_STATUS_BSY = 0x80;
  static constexpr std::uint32_t TFD_STS_STATUS_DRQ = 0x08;
  // PxSSTS
  static constexpr std::uint32_t SATA_STATUS_IPM  = 0xF00;
  static constexpr std::uint32_t SATA_STATUS_DET  = 0x0F;

  public:
    AhciPort();

    void SetMMIOBase(PortRegisters* base){ m_registers = base;}
    void SetPresent(){m_present = true;}
    bool Present() const {return m_present;}
    bool DmaRunning() const{ return m_dmaRunning;}

    bool HasDevice() const;
    void SetDevicePresent(){m_devicePresent = true;}
    bool DevicePresent() const {return m_devicePresent;}

    // registers
    void SetCLB(std::uint32_t addr){ m_registers->clistb = addr;}
    void SetCLB(std::uint64_t addr){ m_registers->clistb = addr;}
    void SetFisB(std::uint32_t addr){ m_registers->fisb = addr;}
    void SetFisB(std::uint64_t addr){ m_registers->fisb = addr;}

    volatile std::uint32_t& IS(){ return m_registers->is;}
    volatile const std::uint32_t& IS() const{ return m_registers->is;}
    volatile std::uint32_t& IE(){return m_registers->ie;}
    volatile const std::uint32_t& IE() const{return m_registers->ie;}

    volatile std::uint32_t& CommandStatus(){ return m_registers->cmd;}
    volatile const std::uint32_t& CommandStatus() const{ return m_registers->cmd;}

    volatile std::uint32_t& SataActive(){ return m_registers->sact;}
    volatile std::uint32_t& CommandIssue(){ return m_registers->ci;}
    volatile const std::uint32_t& CommandIssue() const{ return m_registers->ci;}

    // a few other registers...
    void ClearSERR();

    // Fis Recieved Enable Bit
    void EnableFRE();
    void DisableFRE();
    bool FisRecievedEnabled() const{ return m_fre;}

    void ClearInterruptStatus(){IS() = 0;}
    void EnableInterrupts(){IE() = ENABLE_ALL_INTS;}
    void DisableInterrupts(){IE() = 0;}

    bool Idle() const;
    void ForceIdle();

    void EnableDMAEngine();

    std::uint64_t CommandListBaseAddr() const;
    CommandList* CommandListPtr() const
    { return reinterpret_cast<CommandList*>(CommandListBaseAddr());}
    int EmptyCommandSlot();


    std::uint8_t NumCommandSlots();
    void IssueCommand(std::uint8_t slot);
    bool CommandSlotSet(std::uint8_t slot) const;

  private:
    std::uint32_t CLBLow() const{return m_registers->clistb;}
    std::uint32_t CLBHigh() const{return m_registers->clistbu;}

  private:
    volatile PortRegisters* m_registers;
    bool m_present;
    bool m_devicePresent;
    bool m_fre;
    bool m_dmaRunning;
};

}
