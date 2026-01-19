#pragma once

#include <cstdint>

#include "ram/options.hpp"

#include "kassert.hpp"
#include "aii/string.h"

#include "memmap/memory_map.hpp"

#include "drivers/serial/kostream.hpp"

#include "types.hpp"

namespace Chisaka::PageAllocators::Ns{

namespace Freelist{

using AddrType = PhysAddr;
static constexpr std::uint16_t PAGESIZE = 0x1000;

struct ListEntry{
  ListEntry* next;
};
static_assert(sizeof(ListEntry) == 8);

//-------------------------------------------------------------

ListEntry& Head();
ListEntry& LowHead();
std::size_t& FreePages();

void Initialise(const MemoryMap memMap) noexcept;

void PrintFreeBases() noexcept;
void PrintFreeBasesLow() noexcept;

inline void* NextFree() noexcept{return Head().next;}
inline void* NextFreeLow() noexcept{return LowHead().next;}
inline ListEntry* NextFree(ListEntry* pagebase) noexcept{return pagebase->next;}

void* AllocatePage() noexcept;
void* AllocatePage(RamOptions options) noexcept;

void Extract(void* pageaddr, unsigned pages) noexcept;

void* AllocatePages(unsigned pages) noexcept;
void* AllocatePages(unsigned pages, RamOptions options) noexcept;
void* AllocatePagesImpl(ListEntry& listHead, unsigned pages) noexcept;

void DeallocPage(void* base) noexcept;
inline void FreePage(void* base) noexcept{DeallocPage(base);}

}

}
