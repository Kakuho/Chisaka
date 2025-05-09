`Aii` aims to be a freestanding, exception free header only library, for the `Chisaka` operating system kernel.

Each data type has a template argument `Allocator` which is `Aii::Allocator` by default. If you want to use `Aii` in your own kernel, and 
want the default memory allocations made with your own custom heap, implement the files in `stub.hpp` according to your own system.  
`Aii::Allocator` is a stateless allocator and calls these stubs in order to allocate memory.

Similarly smart pointer types has a template parameter "D" which defaults to "Aii::DefaultDeleter". Implementing the
stubs will allow "Aii::DefaultDeleter" to call your custom memory resource deallocation function by default.

You can of course, implement your own allocator however you must ensure it meets the requirements of the
`Aii::IsAllocator<T>` concept.
