#include "interrupt_handlers.hpp"

extern "C"{

void sayHi(int x){
  if(x != 0){
    kout << "why this no work" << '\n';
  }
  if(x == 0){
    kout << "this working now" << '\n';
  }
  kout << "the interrupt works now bros" << '\n';
}

void generalIRQ(int x){
  kout << "AN INTERRUPT HAS OCCURED" << '\n'
       << "interrupt code: " << x << '\n'
       << "END OF HANDLING" << '\n';
}

void contextSwitchISR(void* stackaddr){
  kout << intmode::hex;
  std::uint64_t* saddr = reinterpret_cast<std::uint64_t*>(stackaddr);
  // above the stack pointer
  //std::uint64_t* prsp = reinterpret_cast<std::uint64_t*>(saddr + 1);
  std::uint64_t* prdi = reinterpret_cast<std::uint64_t*>(saddr);
  // below the stack pointer
  std::uint64_t* prax = reinterpret_cast<std::uint64_t*>(saddr - 1);
  std::uint64_t* prcx = reinterpret_cast<std::uint64_t*>(saddr - 2);
  std::uint64_t* prdx = reinterpret_cast<std::uint64_t*>(saddr - 3);
  std::uint64_t* prbx = reinterpret_cast<std::uint64_t*>(saddr - 4);
  std::uint64_t* prbp = reinterpret_cast<std::uint64_t*>(saddr - 5);
  std::uint64_t* prsi = reinterpret_cast<std::uint64_t*>(saddr - 6);
  std::uint64_t* prflags = reinterpret_cast<std::uint64_t*>(saddr - 7);
  kout << "AN INTERRUPT HAS OCCURED" << '\n'
       << "stackaddr = " << reinterpret_cast<Mem::kvirtaddr_t>(saddr) << '\n'
       << "rax = " << *prax << '\n'
       << "rbx = " << *prbx << '\n'
       << "rcx = " << *prcx << '\n'
       << "rdx = " << *prdx << '\n'
       << "rsi = " << *prsi << '\n'
       << "rdi = " << *prdi << '\n'
       << "rbp = " << *prbp << '\n'
       //<< "rsp = " << *prsp << '\n'
       << "rflags = " << *prflags << '\n'
       << "we tryna to ctxt sw" << '\n';
  kout << "END OF HANDLING" << '\n';
}

void PageFaultISR(){
  //kassert("Pagefault isr not implemented" && false);
  /*
  memory::paddr64_t faultingAddr = readcr2();
  kout << intmode::hex;
  kout << "PAGEFAULTING" << '\n'
       << "faulting address: " << faultingAddr << '\n';
  // now we need to resolve faulting address
  memory::ExtractPagingIndices(faultingAddr);
  */
}

} // extern "C"
