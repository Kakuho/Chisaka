// routine to reload the segment register, corresponding to the 
// flat memory model
//
//  Reference:
//    RET, CALL 
//
//  Assumption: 
//    On entering, %rdi contains the gdt virtual address

.globl segreg_reload
segreg_reload:
  cli
  // code segment
  // the code segment can only be changed via a far return
  // retfq is for far returns
  pushq $0x08;
  leaq .load_all_other, %rax
  pushq %rax
  retfq
.load_all_other:
  // data segment
  movw $0x10, %ax
  movw %ax, %ds
  movw %ax, %ss
  movw %ax, %es
  movw %ax, %fs
  // task segment
  movw %ax, %gs
  movw $0x28, %ax
  ltrw %ax
  ret
