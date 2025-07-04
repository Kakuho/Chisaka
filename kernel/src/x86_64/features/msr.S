.globl read_ia32_apic_base
.type read_ia32_apic_base, @function
read_ia32_apic_base:
  pushq %rbp
  movq %rsp, %rbp
  xorq %rdx, %rdx
  xorq %rax, %rax
  xorl %ecx, %ecx
  movl $0x1b, %ecx
  rdmsr
  shlq $32, %rdx
  addq %rdx, %rax
  pop %rbp
  ret

.globl write_ia32_apic_base
.type write_ia32_apic_base, @function
write_ia32_apic_base:
  pushq %rbp
  movq %rsp, %rbp
  movl %edi, %edx
  movl %esi, %eax
  xorl %ecx, %ecx
  movl $0x1b, %ecx
  wrmsr
  pop %rbp
  ret
