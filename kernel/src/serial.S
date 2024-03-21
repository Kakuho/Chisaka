.globl outb

outb: 
  movl %edi, %edx
  movl %esi, % eax
  outb %al, %dx
  ret
