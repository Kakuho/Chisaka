#include "kcontext.hpp"

namespace Chisaka{
  void KContext::InitKContext(){
    kernelBase = Limine::kernel_addr_req.response->virtual_base;
    hhdmOffset = Limine::hhdm_request.response->offset;
  }

  PhysAddr KContext::KernelPhysAddr(){
    return kernelBase - hhdmOffset;
  }

  VirtAddr KContext::KernelVirtAddr(){
    return kernelBase;
  }

  VirtAddr KContext::PhysToVirtAddr(PhysAddr paddr){
    return paddr + hhdmOffset;
  }

  PhysAddr KContext::VirtToPhysAddr(VirtAddr vaddr){
    return vaddr - hhdmOffset;
  }
}
