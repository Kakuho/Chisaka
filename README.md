# Chisaka

Chisaka is an attempt at building an operating system, using the C++ programming language. It is targeted towards x86_64 processors. The operating system is at an extremely primitive stage, so do not expect much.

To bootstrap Chisaka, I used the limine bootloader following the limine-cpp-template template.

## Building and Running

 from root, type:
 
	make run

which compiles the operating system to an iso file and spawns a x86_64 processor to run it. More details can be found below.

## Resources

The following lists resources I found helpful in building this OS:

* OSDEV wiki: https://wiki.osdev.org/Main_Page
	
	Contains a goldmine of information, and useful pointers to research for where you want to 
	take your operating system further.

* Intel x86 Processor Documentation: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

	Detailed explanation for all you want to know of intel x86 processors. Get it from the 
	source.
    
* GCC's C++ Standard Library Implementation: https://gcc.gnu.org/onlinedocs/gcc-4.6.2/libstdc++/api/index.html

	Great reference for building standard library implementations.

* Textbooks:

    In general I prefer textbooks which contains theory and also implementation, where the 
    implementation is used to frame the theory. I have found two textbooks I have got along 
    with, those being:

    * Operating Systems Design and Implementation: Design and Implementation. Third Edition (Prentice Hall Software Series).

    * Operating System Design: The Xinu approach.

    OSDI tends to be more encyclopedic and provides surveys of existing algorithms, meanwhile xinu is a fairly simple system to learn and hack on. OSDI contains references to other algorithms which will prove useful when experimenting.
      

# Limine C++ Template

This repository will demonstrate how to set up a basic x86-64 kernel in C++ using Limine.

It is recommended to cross reference the contents of this repository with [the Limine Bare Bones](https://wiki.osdev.org/Limine_Bare_Bones) OSDev wiki page.

## How to use this?

### Dependencies

Any `make` command depends on GNU make (`gmake`) and is expected to be run using it. This usually means using `make` on most GNU/Linux distros, or `gmake` on other non-GNU systems.

All `make all*` targets depend on a GNU-compatible C++ toolchain capable of generating x86-64 ELF objects. Usually `gcc(g++)/binutils` or `clang++/llvm/lld` provided by any x86-64 UNIX like (including Linux) distribution will suffice.

Additionally, building an ISO with `make all` requires `xorriso`, and building a HDD/USB image with `make all-hdd` requires `sgdisk` (usually from `gdisk` or `gptfdisk` packages) and `mtools`.

### Makefile targets

Running `make all` will compile the kernel (from the `kernel/` directory) and then generate a bootable ISO image.

Running `make all-hdd` will compile the kernel and then generate a raw image suitable to be flashed onto a USB stick or hard drive/SSD.

Running `make run` will build the kernel and a bootable ISO (equivalent to make all) and then run it using `qemu` (if installed).

Running `make run-hdd` will build the kernel and a raw HDD image (equivalent to make all-hdd) and then run it using `qemu` (if installed).

The `run-uefi` and `run-hdd-uefi` targets are equivalent to their non `-uefi` counterparts except that they boot `qemu` using a UEFI-compatible firmware.

