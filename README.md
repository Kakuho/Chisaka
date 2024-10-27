# Chisaka

Chisaka is an attempt at building an operating system, using the C++ programming language. It is targeted towards x86_64 processors. The operating system is at an extremely primitive stage, so do not expect much.

To bootstrap Chisaka, I used the limine bootloader following the limine-cpp-template template.

## Building and Running

from root, type:
 
	make run

There are also other make file commands which are mostly for debugging purposes. 

# Dependencies

There is an explicit dependency on my header only library Primrose.
The compiler assumes there is a symbol link to Primrose in kernel/src, so if you would like to try out the kernel write:



which compiles the operating system to an iso file and spawns a x86_64 processor to run it. More details can be found below.
