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

.globl readcr3
.type readcr3, @function
readcr3:
  xorq %rax, %rax
  movq %cr3, %rax 
  ret

.globl readcr4
.type readcr4, @function
readcr4:
  xorq %rax, %rax
  movq %cr4, %rax 
  ret
