#pragma once

// namespace to store and configure your global kernel managers

#include <cstddef>
#include "limine/utility.hpp"
#include "types.hpp"

namespace Chisaka{
  class KContext{
    public:
      static KContext& Get(){ static KContext g; return g;}

      void InitKContext(){
        kernelBase = Limine::kernel_addr_req.response->virtual_base;
        hhdmOffset = Limine::hhdm_request.response->offset;
      }

      PhysAddr KernelPhysAddr(){
        return kernelBase - hhdmOffset;
      }

      VirtAddr KernelVirtAddr(){
        return kernelBase;
      }

      VirtAddr PhysToVirtAddr(PhysAddr paddr){
        return paddr + hhdmOffset;
      }

      PhysAddr VirtToPhysAddr(VirtAddr vaddr){
        return vaddr - hhdmOffset;
      }

    private:
      VirtAddr kernelBase;
      size_t hhdmOffset;
  };
}
