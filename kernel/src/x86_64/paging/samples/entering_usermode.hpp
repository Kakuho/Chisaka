#include <cstdint>

#include "kcontext.hpp"
#include "x86_64/x86pages.hpp"

#include "drivers/serial/kostream.hpp"

extern "C" void jump_usermode();

extern "C" void test_user_function();

namespace X8664::Paging::Samples{
  // setup up user mode
  // you can get more frames by doing page offsets to 0x100000000;
  inline void EnterUserMode(){
    constexpr std::uintptr_t NEW_PML4_FRAME = 0x100001000;

    auto ClearPageFrame = [](std::uintptr_t frame){
      memset(reinterpret_cast<void*>(frame), 0x1000, static_cast<char>(0));
    };

    kout << intmode::hex << "Test User Function: " << reinterpret_cast<std::uintptr_t>(&test_user_function) << '\n';

    ClearPageFrame(NEW_PML4_FRAME);
    X8664::X86Pages newPages{reinterpret_cast<X8664::UpperPageTable*>(NEW_PML4_FRAME)};

    std::uint64_t cr3 = get_cr3();
    X8664::X86Pages liminePages{reinterpret_cast<X8664::UpperPageTable*>(cr3)};

    newPages.Pml4()->GetEntry(0x100) = PageEntry::CreateSupervisor(liminePages.Pml4()->GetEntry(0x100));
    newPages.Pml4()->GetEntry(0x1ff) = PageEntry::CreateSupervisor(liminePages.Pml4()->GetEntry(0x1ff));
    newPages.Pml4()->GetEntry(0x100).SetUserAccessible(true);
    newPages.Pml4()->GetEntry(0x1ff).SetUserAccessible(true);
    newPages.PrintIndicies(0xffff80007ff54f60);

    auto pdptk = reinterpret_cast<UpperPageTable*>(newPages.Pml4()->GetEntry(0x100).TablePtr());
    pdptk->GetEntry(0x1).SetUserAccessible(true);

    auto pdk = reinterpret_cast<UpperPageTable*>(pdptk->GetEntry(0x1).TablePtr());
    pdk->GetEntry(0x1ff).SetUserAccessible(true);

    auto ptk = reinterpret_cast<PageTable*>(pdk->GetEntry(0x1ff).TablePtr());
    ptk->GetEntry(0x154).SetUserAccessible(true);

    //////////////////////////////////////////////////////////////////////

    newPages.PrintIndicies(0xffffffff80011fc9);

    auto pdpt = reinterpret_cast<UpperPageTable*>(newPages.Pml4()->GetEntry(0x1ff).TablePtr());
    pdpt->GetEntry(0x1fe).SetUserAccessible(true);

    auto pd = reinterpret_cast<UpperPageTable*>(pdpt->GetEntry(0x1fe).TablePtr());
    pd->GetEntry(0x0).SetUserAccessible(true);

    auto pt = reinterpret_cast<PageTable*>(pd->GetEntry(0x0).TablePtr());
    pt->GetEntry(0xB).SetUserAccessible(true);
    pt->GetEntry(0x11).SetUserAccessible(true);
    pt->GetEntry(0x12).SetUserAccessible(true);
    //pt->GetEntry(0x0).SetUserAccessible(true);
    //pt->GetEntry(0x14).SetUserAccessible(true);

    //////////////////////////////////////////////////////////////////////

    newPages.PrintIndicies(0xffffffff800120c1);
    newPages.PrintIndicies(0xffffffff8000b7e2);
    newPages.PrintIndicies(0xffffffff80000418);
    newPages.PrintIndicies(0xffffffff80014b83);


    set_cr3(NEW_PML4_FRAME);
    jump_usermode();
  }
}
