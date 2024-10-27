.globl load_gdtr
load_gdtr:
  LGDT (%rdi) // no offset, use rax as our base
  ret
