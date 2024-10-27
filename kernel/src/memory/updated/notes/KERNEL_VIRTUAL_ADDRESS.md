For now, assume the kernel is always mapped at the top half of the virtual address space.

I also assume the system has 8GiB RAM

I will set up two mappings: 
  
0x000000000           -> 0x1ffffffff            (Identity mapping of the first 8GiB of physical RAM)

0xffffffff80000000    -> 0x1000000017fffffff    (Identity mapping of the first 8GiB of RAM to Kernel hhdm Offset)
