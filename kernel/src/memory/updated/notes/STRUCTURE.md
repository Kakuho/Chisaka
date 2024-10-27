Directory Structure:

memory/

    address.hpp                     - functions relating to address translation, 
                                      memory address types etc

    boot.hpp                        - functions relating to the memory mapping 
                                      at boot

    paging/
        
        structures.hpp              -   file containing paging structures

    vmm/

        paging_table.hpp            -   The paging table of a particular process

    pmm/                            -   Physical Memory Management Code

        pmm.hpp                     - concept for all allocator to 
                                      implement

        page_frame_descriptor.hpp   - useful for buddy
        
        buddy/
            
            buddy_allocator.hpp
            buddy_allocator.cpp

        bitmap/

            bitmap_allocator.hpp
            bitmap_allocator.cpp

        bootstrap/
        
            bootstrap_pmm.hpp       - Used just for initialising the othe pmms
