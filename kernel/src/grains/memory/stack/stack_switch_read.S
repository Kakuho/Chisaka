.globl stack_switch_read_memory
stack_switch_read_memory:
movq -16(%rsp), %rax
ret

.globl stack_switch_dump_memory
stack_switch_dump_memory:
movq (%rsp), %rax
pushq $10
pushq $45
pushq %rax
ret

