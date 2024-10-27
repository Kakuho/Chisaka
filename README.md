# Chisaka

Chisaka is an attempt at building an operating system, using the C++ programming language. It is targeted towards x86_64 processors. The operating system is at an extremely primitive stage, so do not expect much.

To bootstrap Chisaka, I used the limine bootloader following the limine-cpp-template template. A link can be found here: https://github.com/limine-bootloader/limine-cxx-template

## Building and Running

### Dependencies

There is an explicit dependency on my header only library ```Primrose```.
The compiler assumes there is a symbol link to ```Primrose``` in ```kernel/src```, so if you would like to try out the kernel follow these steps:

* cd into ```kernel/src```
* add a symbol link to ```Primrose/include``` 

You should now be able to compile the kernel and run the image.

### Trying out Chisaka

To compile the kernel and run the image, type ```make run``` from root.

There are also other make file commands which are mostly for debugging purposes, you can find these by browsing the makefile. 
