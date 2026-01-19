#include <cstdint>

// these function implementations are 
// required for using static locals in freestanding environments
// not sure how im supposed to init tbh lolol

extern "C" int __cxa_guard_acquire(std::uint64_t* gv) {
  /* TO DO */
  return 0;
}

extern "C" int __cxa_guard_release(std::uint64_t* gv) {
  /* TO DO */
}
