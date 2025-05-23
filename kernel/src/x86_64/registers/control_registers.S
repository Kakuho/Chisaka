.globl readcr0
.type readcr0, @function
readcr0:
  xorq %rax, %rax
  movq %cr0, %rax # rax is 64 bit register
  ret

.globl writecr0
.type writecr0, @function
writecr0:
  movq %rdi, %cr0
  ret;

.globl readcr2
.type readcr2, @function
readcr2:
  xorq %rax, %rax
  movq %cr2, %rax 
  ret

.globl get_cr3
.type get_cr3, @function
get_cr3:
  xorq %rax, %rax
  movq %cr3, %rax 
  ret

.globl set_cr3
.type set_cr3, @function
set_cr3:
  movq %rdi, %cr3
  ret

.globl readcr4
.type readcr4, @function
readcr4:
  xorq %rax, %rax
  movq %cr4, %rax 
  ret
