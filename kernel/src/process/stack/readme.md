I originally tried to change the global stack... however I ran into problems

Another idea is to make a stack entry point, enter into that particular stack, and then perform
calls and rets on that stack.

Thus the control flow would look like:

                                - - - - kernel process stack 0 - - - - >
                              |
                             
                              |
- - - - kernel stack - - - -  > - - - - kernel process stack 1 - - - - >
                              | 
                              
                              |
                                - - - - kernel process stack 2 - - - - >

the problem then ... like how my previous attempt to change the global stack ... 
how do we save the position of the top of kernel process stack?

I.e, suppose we are executing a process.

- - - - Process Stack - - - - >

                                - - - - kernel process stack 0 - - - - >
                              |
                             
                              |
- - - - kernel Stack - - - -  > - - - - kernel process stack 1 - - - - >
                              | 
                              
                              |
                                - - - - kernel process stack 2 - - - - >

The process now requires using kernel services, via a syscall or otherwise.

The kernel should be able to assign a useable [kernel process stack] to that 
process and execution should proceed within that stack.

Let the stack that is selected be [kernel process stack 1], then the control flow 
should look like:

- - - - Process Stack - - - - > |                                          | - - - - Process Stack - - - - >
                                |                                          |
                                | - - - - kernel process stack 1 - - - - > |

The system then performs the kernel service using the allocated stack and returns 
back to the process when it is finished.

Thus, first we start the thread of execution using the process stack, then we switch 
to a kernel stack when we require kernel service, and finally on return, we go back to 
the process stack.
