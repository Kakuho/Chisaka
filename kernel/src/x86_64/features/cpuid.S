// Procedures which implements cpuid specific functions
// for now I'll push the multiple return values onto the stack
//
// Each procedure returns a pointer to the top of stack,
// everything below that pointer are the return values of cpuid.
//
// I feel like i could write a procedure with parameters for this...

.globl cpuid_0
.type cpuid_0, @function
cpuid_0:
  // Todo : Maxmimum input value for 
  // basic cpu functionalities
  pushq %rbp
  movq %rsp, %rbp
  xorq %rax, %rax
  cpuid
  movl %eax, -4(%rbp)
  movl %ebx, -8(%rbp)
  movl %ecx, -12(%rbp)
  movl %edx, -16(%rbp)
  movq %rbp, %rax
  pop %rbp
  ret

.globl cpuid_1
.type cpuid_1, @function
cpuid_1:
  pushq %rbp
  movq %rsp, %rbp
  xorq %rax, %rax
  cpuid
  movl %eax, -4(%rbp)
  movl %ebx, -8(%rbp)
  movl %ecx, -12(%rbp)
  movl %edx, -16(%rbp)
  movq %rbp, %rax
  pop %rbp
  ret

.globl cpuid_80000008
.type cpuid_80000008, @function
cpuid_80000008:
  pushq %rbp
  movq %rsp, %rbp
  xorq %rax, %rax
  movl $0x80000008, %eax
  cpuid
  movl %eax, -4(%rbp)
  movl %ebx, -8(%rbp)
  movq %rbp, %rax
  pop %rbp
  ret

.globl cpuid_80000008_eax
.type cpuid_80000008_eax, @function
cpuid_80000008_eax:
  pushq %rbp
  movq %rsp, %rbp
  xorq %rax, %rax
  movl $0x80000008, %eax
  cpuid
  pop %rbp
  ret
