.globl load_idtr
load_idtr:
  LIDT (%rdi)
  ret

//-------------------------------------------------------------
// Interrupt Setting / Disabling
//-------------------------------------------------------------

.globl enable_interrupts
enable_interrupts:
  sti
  ret

.globl disable_interrupts
disable_interrupts:
  cli
  ret

//---------------------------------------------------------------/
// Interrupt Handlers
//-------------------------------------------------------------

.globl isr_0
isr_0:
  movq $0, %rdi
  call generalIRQ
  iretq

.globl isr_1
isr_1:
  movq $1, %rdi
  call generalIRQ
  iretq

.globl isr_2
isr_2:
  movq $2, %rdi
  call generalIRQ
  iretq

.globl isr_3
isr_3:
  movq $3, %rdi
  call generalIRQ
  iretq

.globl isr_4
isr_4:
  movq $4, %rdi
  call generalIRQ
  iretq

.globl isr_5
isr_5:
  movq $5, %rdi
  call generalIRQ
  iretq

.globl isr_6
isr_6:
  movq $6, %rdi
  call generalIRQ
  iretq

.globl isr_7
isr_7:
  movq $7, %rdi
  call generalIRQ
  iretq

.globl isr_8
isr_8:
  movq $8, %rdi
  call generalIRQ
  iretq

.globl isr_9
isr_9:
  movq $9, %rdi
  call generalIRQ
  iretq

.globl isr_10
isr_10:
  movq $10, %rdi
  call generalIRQ
  iretq

.globl isr_11
isr_11:
  movq $11, %rdi
  call generalIRQ
  iretq

.globl isr_12
isr_12:
  movq $12, %rdi
  call generalIRQ
  iretq

.globl isr_13
isr_13:
  cld
  movq $13, %rdi
  call generalIRQ
  add $16, %rsp
  iretq

.globl isr_14
isr_14:
  movq $14, %rdi
  call generalIRQ
  iretq

.globl isr_15
isr_15:
  movq $15, %rdi
  call generalIRQ
  iretq

.globl isr_16
isr_16:
  movq $16, %rdi
  call generalIRQ
  iretq

.globl isr_17
isr_17:
  movq $17, %rdi
  call generalIRQ
  iretq

.globl isr_18
isr_18:
  movq $18, %rdi
  call generalIRQ
  iretq

.globl isr_19
isr_19:
  movq $19, %rdi
  call generalIRQ
  iretq

.globl isr_20
isr_20:
  movq $20, %rdi
  call generalIRQ
  iretq

.globl isr_21
isr_21:
  movq $21, %rdi
  call generalIRQ
  iretq

.globl isr_22
isr_22:
  movq $22, %rdi
  call generalIRQ
  iretq

.globl isr_23
isr_23:
  movq $23, %rdi
  call generalIRQ
  iretq

.globl isr_24
isr_24:
  movq $24, %rdi
  call generalIRQ
  iretq

.globl isr_25
isr_25:
  movq $25, %rdi
  call generalIRQ
  iretq

.globl isr_26
isr_26:
  movq $26, %rdi
  call generalIRQ
  iretq

.globl isr_27
isr_27:
  movq $27, %rdi
  call generalIRQ
  iretq

.globl isr_28
isr_28:
  movq $28, %rdi
  call generalIRQ
  iretq

.globl isr_29
isr_29:
  movq $29, %rdi
  call generalIRQ
  iretq

.globl isr_30
isr_30:
  movq $30, %rdi
  call generalIRQ
  iretq

.globl isr_31
isr_31:
  movq $31, %rdi
  call generalIRQ
  iretq

.globl isr_32
isr_32:
  movq $32, %rdi
  call generalIRQ
  iretq

.globl isr_33
isr_33:
  movq $33, %rdi
  call generalIRQ
  iretq

.globl isr_ctxtsw
isr_ctxtsw:
  cli
  // maybe we can push %rdi, to save its value, then move stack pointer into 
  // %rdi.
  pushq %rdi // confused on pushing stack pointer
  movq %rsp, %rdi
  // pushing
  pushq %rax
  pushq %rcx
  pushq %rdx
  pushq %rbx
  pushq %rbp
  pushq %rsi
  pushfq
  // handling
  call contextSwitchISR
  // popping
  popfq
  popq %rsi
  popq %rbp
  popq %rbx
  popq %rdx
  popq %rcx
  popq %rax
  popq %rdi // confused on pushing stack pointer
  iretq

.globl isr_pf
isr_pf:
  cli
  // maybe we can push %rdi, to save its value, then move stack pointer into 
  // %rdi.
  pushq %rdi // confused on pushing stack pointer
  movq %rsp, %rdi
  // pushing
  pushq %rax
  pushq %rcx
  pushq %rdx
  pushq %rbx
  pushq %rbp
  pushq %rsi
  pushfq
  // handling
  call PageFaultISR
  // popping
  popfq
  popq %rsi
  popq %rbp
  popq %rbx
  popq %rdx
  popq %rcx
  popq %rax
  popq %rdi // confused on pushing stack pointer
  iretq

.globl isr
isr:
  .quad isr_0
  .quad isr_1
  .quad isr_2
  .quad isr_3
  .quad isr_4
  .quad isr_ctxtsw      // NOTE ISR 5 IS MAPPED TO isr_ctxtsw for now!!! 
  //.quad isr_5
  .quad isr_6
  .quad isr_7
  .quad isr_8
  .quad isr_9
  .quad isr_10
  .quad isr_11
  .quad isr_12
  .quad isr_13
  .quad isr_pf
  .quad isr_15
  .quad isr_16
  .quad isr_17
  .quad isr_18
  .quad isr_19
  .quad isr_20
  .quad isr_21
  .quad isr_22
  .quad isr_23
  .quad isr_24
  .quad isr_25
  .quad isr_26
  .quad isr_27
  .quad isr_28
  .quad isr_29
  .quad isr_30
  .quad isr_31
  .quad isr_32
  .quad isr_ctxtsw

