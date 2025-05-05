#pragma once

//  X86_64 registers for process context switching
//  Design: 
//    * GDT is not changed
//    * LDT is not changed
//    * Segment Registers are not changed

#include <cstdint>

namespace Proc{

struct X8664Pcb{
  std::uint64_t rax;      // General Purpose Registers
  std::uint64_t rbx;
  std::uint64_t rcx;
  std::uint64_t rdx;
  std::uint64_t rsi;
  std::uint64_t rdi;
  std::uint64_t rbp;
  std::uint64_t rsp;
  std::uint64_t r8;
  std::uint64_t r9;
  std::uint64_t r10;
  std::uint64_t r11;
  std::uint64_t r12;
  std::uint64_t r13;
  std::uint64_t r14;
  std::uint64_t r15;

  std::uint64_t rflags;     // Status and Instruction
  std::uint64_t rip;

  std::uint64_t cr0;        // Control Registers
  std::uint64_t cr1;
  std::uint64_t cr2;
  std::uint64_t cr3;
  std::uint64_t cr4;
};

} // namespace Proc
