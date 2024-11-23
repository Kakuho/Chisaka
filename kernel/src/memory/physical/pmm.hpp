#include <cstdint>

namespace Mem::Phys{

  // template concept for the underlying page frame allocator implementation
  // allows for static polymorphism
  template <typename T>
    concept PmmImpl = requires(T pmm, std::size_t pages, void* addr){
      pmm.AllocatePage();
      pmm.FreePage(addr);
    };
  
  // this class serves as just an interface to the physical page frame manager.
  // this is because I want to be able to configure the pmm with different allocation 
  // mechanisms
  template<typename T>
    requires PmmImpl<T>
  class Pmm{
    public:

      Pmm() = default;

      // the interface
      void* alloc()             { m_pmm.alloc(); };
      void* alloc(std::size_t n){ m_pmm.alloc(n); };
      void free(void* addr)     {m_pmm.free(addr); }

    private:

      T m_pmm;
  };

}
