//  Routine to Perform stack switching.
//
//  Assumption: 
//    On entering, %rdi contains the stack pointer to switch to

.globl read_rsp
read_rsp:
movq %rsp, %rax
ret

# this switches rsp to the correct value, however on exit the calling function 
# performs a leave which sets the rsp to the calling functions pushed %rbp
.globl switch_stack
switch_stack:
movq (%rsp), %rax
movq %rdi, %rsp
pushq %rax
movq %rdi, %rbp
ret

# I think I need to emulate the calling conventions of function call
# exit mechanism in order for this to work...
.globl switch_stack
emulated_switch_stack:
movq (%rsp), %rax
movq %rdi, %rsp
pushq %rax
movq %rdi, %rbp
ret
