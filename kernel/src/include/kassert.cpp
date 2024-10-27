#include "kassert.hpp"

void kassert_fail_debug_impl(
  const char* functionName, 
  const char* filename, 
  std::size_t lineNumber,
  const char* expression
){
  kout << "KASSERT FAILURE\n";
  kout << filename << " :: " << 
          functionName << "() ::  "<< 
          "line " << intmode::dec << lineNumber << '\n';
  kout << "assertion '" << expression << "' failed" << '\n';
  kout << "halting..." << '\n';
  X8664::HaltCatchFire();
}

void kassert_fail_impl(
  const char* filename, 
  std::size_t lineNumber
){
  kout << "KASSERT FAILURE\n";
  kout << filename << " :: " << 
          "line " << intmode::dec << lineNumber << '\n';
  kout << "halting..." << '\n';
  X8664::HaltCatchFire();
}
