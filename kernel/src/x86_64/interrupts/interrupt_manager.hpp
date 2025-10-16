#ifndef INTERRUPT_MANAGER_HPP
#define INTERRUPT_MANAGER_HPP

// This class should represent the focal point for interrupts,
// the kernel should interface with this class in order to modify 
// interrupt code

#include "structs/idt_table.hpp"
#include "structs/idt_descriptor.hpp"
#include "structs/gate_descriptor.hpp"
#include "structs/attributes.hpp"
#include "constants.hpp"

// some functionalities from idt_routine.S

extern "C"{
  void enable_interrupts();
  void disable_interrupts();
}

extern "C" void(*isr[])();

constexpr void Null(){}

namespace X8664{

class InterruptManager{
  public:
    //-------------------------------------------------------------
    //  Lifetime
    //-------------------------------------------------------------

    explicit InterruptManager();
  
  private:
    void InitialiseGates();

  public:
    //-------------------------------------------------------------
    //  Interrupt Service Requests
    //-------------------------------------------------------------


    void InstallISR(std::uint16_t index, void(*handler)());
    void InstallISR(std::uint16_t index, void* handler);
    void InstallISR(std::uint16_t index, std::uint64_t handler);

    // fetching the interrupt gates
    
    [[nodiscard]] 
    GateDescriptor& InterruptGate(std::size_t index) noexcept{
      kassert(index < 256);
      return m_table[index];
    }

    [[nodiscard]] 
    const GateDescriptor& InterruptGate(std::size_t index) const noexcept{
      kassert(index < 256);
      return m_table[index];
    };

    //-------------------------------------------------------------
    //  Operational
    //-------------------------------------------------------------

    void LoadIdtr(){m_idtr.LoadCurrent();}
    void EnableInterrupts(){enable_interrupts();}

  private:
    IdtTable m_table;
    IdtDescriptor m_idtr;
};

} // namespace X86664

#endif
