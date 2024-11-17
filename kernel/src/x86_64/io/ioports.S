.globl outb
outb: 
  movl %edi, %edx
  movl %esi, %eax
  outb %al , %dx
  ret

.globl outw
outw: 
  movl %edi, %edx
  movl %esi, %eax
  outw %ax, %dx
  ret

.globl outl
outl: 
  movl %edi, %edx
  movl %esi, %eax
  outl %eax, %edx
  ret

.globl inb
.type inb, @function
inb:
  movl %edi, %edx
  inb %dx, %al
  ret

.globl inl
inl:
  movl %edi, %edx
  inl %edx, %eax
  ret
