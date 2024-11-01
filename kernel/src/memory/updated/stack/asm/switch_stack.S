//  Routine to Perform stack switching.
//
//  Assumption: 
//    On entering, %rdi contains the stack pointer to switch to

.globl read_rsp
read_rsp:
movq %rsp, %rax
ret

.globl switch_stack
switch_stack:
movq %rdi, %rsp
ret
