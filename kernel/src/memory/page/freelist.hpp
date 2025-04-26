#pragma once

#include <cstdint>

#include "options.hpp"

#include "lib/kassert.hpp"
#include "lib/string.h"

#include "./../address.hpp"
#include "./../alignment.hpp"
#include "./../memory_map/memory_map_descriptor.hpp"

#include "firmware/limine/limine.h"
#include "firmware/limine/requests.hpp"
#include "drivers/serial/kostream.hpp"
#include "lib/string.h"

namespace Mem::Page::Freelist{

using AddrType = kvirtaddr_t;
static constexpr std::uint16_t PAGESIZE = 0x1000;

struct ListEntry{
  ListEntry* next;
};
static_assert(sizeof(ListEntry) == 8);

//-------------------------------------------------------------

ListEntry& Head();
ListEntry& LowHead();
std::size_t& FreePages();

void Initialise(const MemoryMapDescriptor& memMap) noexcept;

void PrintFreeBases() noexcept;
void PrintFreeBasesLow() noexcept;

inline void* NextFree() noexcept{return Head().next;}
inline void* NextFreeLow() noexcept{return LowHead().next;}
inline ListEntry* NextFree(ListEntry* pagebase) noexcept{return pagebase->next;}

void* AllocatePage() noexcept;
void* AllocatePage(OPT options) noexcept;

void Extract(void* pageaddr, unsigned pages) noexcept;

void* AllocatePages(unsigned pages) noexcept;
void* AllocatePages(unsigned pages, OPT options) noexcept;
void* AllocatePagesImpl(ListEntry& listHead, unsigned pages) noexcept;

void DeallocPage(void* base) noexcept;
inline void FreePage(void* base) noexcept{DeallocPage(base);}

} // namespace Mem::Page::Freelist
