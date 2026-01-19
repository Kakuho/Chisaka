#include "freelist.hpp"
#include "aii/string.h"

namespace{
  using namespace Chisaka::PageAllocators::Ns::Freelist;
  ListEntry head;   
  ListEntry lowHead;
  std::size_t freePages;
}

namespace Chisaka::PageAllocators::Ns::Freelist{

ListEntry& Head(){ return head;}

ListEntry& LowHead(){ return lowHead;}

std::size_t& FreePages(){ return freePages;}

// Initialisation Routine 

void InitialiseList(const MemoryMap& memMap) noexcept;
void InitialiseLowList(const MemoryMap& memMap) noexcept;
AddrType SetupRegion(AddrType base, std::size_t length, AddrType lastAddr);

void Initialise(const MemoryMap& memMap) noexcept{
  FreePages() = 0;
  InitialiseList(memMap);
  InitialiseLowList(memMap);
}

void InitialiseList(const MemoryMap& memMap) noexcept{
  // Initialisation Routine, sets up the free list entries
  bool firstIteration = true;
  bool crossedEntry = false;
  AddrType lastAddr = 0;
  for(std::uint8_t i = 0; i < memMap.Entries(); i++){
    const MemoryMapEntry& entry = memMap.Entry(i);
    if(entry.base > 0xFFFF'FFFF){
      if(entry.type == MemoryMapEntry::Type::Useable){
        if(firstIteration){
          Head().next = reinterpret_cast<ListEntry*>(entry.base);
          firstIteration = false;
          lastAddr = SetupRegion(entry.base, entry.length, 0);
        }
        else{
          lastAddr = SetupRegion(entry.base, entry.length, lastAddr);
        }
      }
    }
  }
  ListEntry* lastEntry = reinterpret_cast<ListEntry*>(lastAddr);
  lastEntry->next = nullptr;
}

void InitialiseLowList(const MemoryMap& memMap) noexcept{
  // initialise the low 32-bit physical memories
  bool firstIteration = true;
  bool crossedEntry = false;
  AddrType lastAddr = 0;
  for(std::uint8_t i = 0; i < memMap.Entries(); i++){
    const MemoryMapEntry& entry = memMap.Entry(i);
    // assuming entries are physical
    if(entry.base + entry.length <= 0xFFFF'FFFF){
      if(entry.type == MemoryMapEntry::Type::Useable){
        if(firstIteration){
          LowHead().next = reinterpret_cast<ListEntry*>(entry.base);
          firstIteration = false;
          lastAddr = SetupRegion(entry.base, entry.length, 0);
        }
        else{
          lastAddr = SetupRegion(entry.base, entry.length, lastAddr);
        }
      }
    }
  }
  ListEntry* lastEntry = reinterpret_cast<ListEntry*>(lastAddr);
  lastEntry->next = nullptr;
};

AddrType SetupRegion(AddrType base, std::size_t length, AddrType lastAddr){
  AddrType current = base;
  if(lastAddr){
    ListEntry* prevEntry = reinterpret_cast<ListEntry*>(lastAddr);
    prevEntry->next = reinterpret_cast<ListEntry*>(base);
  }
  while(current < base + length){
    if(current + PAGESIZE < base + length){
      ListEntry* currentEntry = reinterpret_cast<ListEntry*>(current);
      currentEntry->next = reinterpret_cast<ListEntry*>(current + PAGESIZE);
      FreePages()++;
    }
    current += PAGESIZE;
  }
  return current - PAGESIZE;
}

void PrintFreeBases() noexcept{
  ListEntry* current = Head().next;
  while(current != nullptr){
    current = current->next;
    kout << reinterpret_cast<std::uint64_t>(current) << '\n';
  }
}

void PrintFreeBasesLow() noexcept{
  ListEntry* current = LowHead().next;
  while(current != nullptr){
    current = current->next;
    kout << reinterpret_cast<std::uint64_t>(current) << '\n';
  }
}

// AllocatePage

// forward declare to make it easier to  read
void* AllocatePageImpl(ListEntry& listHead) noexcept;

void* AllocatePage() noexcept{ 
  return AllocatePageImpl(Head());
}

void* AllocatePage(RamOptions options) noexcept{
  void* allocd = nullptr;
  if(options == RamOptions::LOW){
    allocd = AllocatePageImpl(LowHead());
  }
  else{
    allocd = AllocatePageImpl(Head());
  }
  if(allocd && options == RamOptions::Z){
    Aii::Memset(allocd, 0, PAGESIZE);
  }
  return allocd;
}

void* AllocatePageImpl(ListEntry& listHead) noexcept{
  void* allocd = listHead.next;
  if(allocd){
    listHead.next = listHead.next->next;
  }
  else{
    listHead.next = nullptr;
  }
  return allocd;
}

// Extract(void*, unsigned)

void ExtractImpl(ListEntry& listHead, void* pageaddr, unsigned pages) noexcept;

void Extract(void* pageaddr, unsigned pages) noexcept{
  // Extract pages number of pages starting at address pageaddr
  // Assumes pageaddr is within the page
  if(reinterpret_cast<std::uintptr_t>(pageaddr) <= 0xFFFF'FFFF){
    ExtractImpl(LowHead(), pageaddr, pages);
  }
  else{
    ExtractImpl(Head(), pageaddr, pages);
  }
}

void ExtractImpl(ListEntry& listHead, void* pageaddr, unsigned pages) noexcept{
  // Assumes pageaddr is on the list of listHead
  ListEntry* current = listHead.next;
  if(!current){
    return;
  }
  if(current == pageaddr){
    // we carve out the front pages
    ListEntry* end = reinterpret_cast<ListEntry*>(pageaddr);
    for(auto i = 0u; i < pages; i++){
      end = NextFree(end);
    }
    listHead.next = end;
  }
  ListEntry* next = current->next;
  if(!next){
    return;
  }
  // end of check, start the iteration process
  while(next < pageaddr){
    current = next;
    next = current->next;
  }
  if(next > pageaddr){
    // there is no entry starting with page addr within the freelist
    return;
  }
  // main update loop
  kassert(next == pageaddr);
  // now we need to set current->next to be the page with base pageaddr+pages
  ListEntry* end = reinterpret_cast<ListEntry*>(pageaddr);
  for(auto i = 0u; i < pages; i++){
    end = NextFree(end);
  }
  current->next = end;
  return;
}

// AllocatePages(unsigned)

void* AllocatePagesImpl(ListEntry& listHead, unsigned pages) noexcept;

void* AllocatePages(unsigned pages) noexcept{ 
  return AllocatePagesImpl(Head(), pages);
}

void* AllocatePages(unsigned pages, RamOptions options) noexcept{
  void* allocd = nullptr;
  if(options == RamOptions::LOW){
    allocd = AllocatePagesImpl(LowHead(), pages);
  }
  else{
    allocd = AllocatePagesImpl(Head(), pages);
  }
  if(allocd && options == RamOptions::Z){
    Aii::Memset(allocd, 0, pages*PAGESIZE);
  }
  return allocd;
}

void* AllocatePagesImpl(ListEntry& listHead, unsigned pages) noexcept{
  // page map can keep count of how many pages the allocated block span
  // this makes it easier for deallocation
  unsigned count = 1;
  void* allocBase = listHead.next;
  ListEntry* currentEntry = reinterpret_cast<ListEntry*>(allocBase);
  if(!currentEntry){
    return nullptr;
  }
  ListEntry* nextEntry = currentEntry->next;
  if(!nextEntry){
    return nullptr;
  }
  while(count < pages){
    // check for contigousity
    bool isContigous = reinterpret_cast<std::uintptr_t>(currentEntry) + PAGESIZE == 
        reinterpret_cast<std::uintptr_t>(nextEntry);
    if(isContigous)
    {
      count++;
      if(count == pages) break;
    }
    else{
      count = 1;
      allocBase = nextEntry;
    }
    // update the iterators
    currentEntry = currentEntry->next;
    if(!currentEntry){ 
      return nullptr;
    }
    nextEntry = nextEntry->next;
    if(!nextEntry){ 
      return nullptr;
    }
  }
  // only reaches here if pages amount of contigous pages is found
  ExtractImpl(listHead, allocBase, pages);
  return allocBase;
}

void DeallocPage(void* base) noexcept{
  // Assumption base != head of any lists
  ListEntry* allocd = reinterpret_cast<ListEntry*>(base);
  ListEntry*& currentHead = Head().next;
  if(!currentHead){
    currentHead = allocd;
    currentHead->next = nullptr;
  }
  else if(allocd < currentHead){
    ListEntry* oldHead = Head().next;
    currentHead = allocd;
    // fetch how many pages the allocated page block spans by using the page map 
    // since it might be a non zero block of pages allocated by AllocatePages(unsigned)
    currentHead->next = oldHead; // todo: update this with regards to the page map
  }
  else if(!currentHead->next){
    currentHead->next = allocd;
  }
  else{
    kassert(allocd > currentHead);
    kassert(currentHead->next);
    // main iteration loop
    ListEntry* current = currentHead;         // is not nullptr at this point
    ListEntry* next = currentHead->next;      // is not nullptr at this point
    while(allocd > next){
      current = next;
      next = current->next;
      if(!next) break;
    }
    current->next = allocd;
    allocd->next = next;
  }
}

}
