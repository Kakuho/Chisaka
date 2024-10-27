.globl set_paging_table
set_paging_table:
movq %rdi, %cr3
ret

.globl load_paging_table
load_paging_table:
movq %cr3, %rax
ret
