.globl readcr4
.type readcr4, @function
readcr4:
  movq %cr4, %rax # rax is 32 bit register
  ret

.globl readcr3
.type readcr3, @function
readcr3:
  movq %cr3, %rax # rax is 32 bit register
  ret
