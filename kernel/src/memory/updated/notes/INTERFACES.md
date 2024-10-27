memory.hpp/.cpp

Types:

    physaddr:   u64
    virtaddr:   u64

Address Related:

    physaddr vaddrToPaddr(virtaddr)
    virtaddr paddrToVaddr(physaddr)
    void PrintKernelAddress()
    virtaddr GetKernelVirtAddress()     - Returns the kernel's base virtual address

Memory Mapping:

    void PrintMemoryMap()               - Prints Limine Memory map
    size_t CalculateAvailableMemory()   - counts number of available mememory bytes 
    physaddr UpperLimitProbe()          - Returns the very last physical address of usable ram
    physaddr LowerLimitProbe()          - Returns the first phys address of usable ram

Paging Related:

    virtaddr FormLinearAddress()        - Returns virtual address made up of the paging entries
    void PrintPagingEntries(virtaddr)   - Prints the Paging Entries making up a linear address
    void PrintPageFrames()              - Prints the available page frames in 'page: phys_base -> phys_end' format

Probing (Examples?):

    void ProbeMemoryRegion(index)       - probes a single usable memory region given from us by Limine

Misc:

    void CORRUPTMEMORY()                - literally thrashes the ram

// ------------------------------------------------------ //

pages.hpp/.cpp

Types:

    PageFrameDescriptor{
        base,
        flags,
        count
    }

        - used to book keep details regarding a book frame
    
    PageFrameDescriptor glbl_pagetor[];     

        -   global buffer to bookkeep page frame descriptors

Functions:

    sz GetBootPages()               - returns the # availabe Pages at Boot

    void InitialisePageFrames()     - They're using an array to keep hold of all information regarding 
                                      pageframes (PageFrameDescriptor[]).
                                      This initialises the global array by setting base, flags and count.

    void PrintSizePagetorMap()      - Prints the size of the global page frame descriptor 

    void PrintAvailablePages()      - Prints the number of available pages in the global page frame descriptor

    sz GetAvailablePages()          - Returns the number of available page frames (glb pgtor)

    void CheckBootPages()           - Checks if the glb pgtor number of pages < number of pageframes available
                                      at boot
                                      (the #page frames descriptor in glb pgtor is fixed constant, and not calculated)  

    void PrintPageFrames()          - Prints details regarding the managed page frames, using the glbl pagetor

    void PageAddress(frameIndex)    - Print the phys address of a pftor using the glbl pagetor

    paddr VirtualToPageBase(vaddr)  - Returns the physaddr base of the page where the current virtaddr references

These all references the global page frame descriptor array
