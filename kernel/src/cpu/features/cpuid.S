// Procedures which implements cpuid specific functions
// for now I'll push the multiple return values onto the stack

.globl cpuid_0
.type cpuid_0, @function
cpuid_0:
  pushq %rbp
  movq %rsp, %rbp
  xorq %rax, %rax
  cpuid
  movq %rbp, %rax
  movl %ebx, -4(%rbp)
  movl %ecx, -8(%rbp)
  movl %edx, -12(%rbp)
  pop %rbp
  ret
