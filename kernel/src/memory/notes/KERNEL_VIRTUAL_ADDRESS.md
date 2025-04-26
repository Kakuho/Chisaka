For now, assume the kernel is always mapped at the top half of the virtual address space.

I also assume the system has 8GiB RAM

//-------------------------------------------------------------

I will set up two mappings: 

0x00000000'00000000 -> 0x00000002'40000000    Identity mapping of the first 9GiB of RAM (9 GiB because some extra memory
                                              is required after the first 8GiB of present RAM)

0xffff8000'00000000 -> 0xffff8002'40000000    Identity mapping of the first 9GiB of RAM with kernel offset applied
