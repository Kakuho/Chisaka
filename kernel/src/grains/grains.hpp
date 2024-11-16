// Grains is the name of my runtime test framework
//
// There should only be one active ...
//
// Assume that a Grain is a non returning function, and the cpu will halt 
// at the end of the test

#include <cstdint>

#include "drivers/serial/kostream.hpp"
#include "x86_64/utilites.hpp"
#include "misc/ascii.hpp"

#define START_GRAIN(grainDescription)            \
  Begin(__func__, __FILE__, grainDescription)    \

#define END_GRAIN()                             \
  End(__func__, __FILE__)                       \

#define GRAIN(grainDescription, ...)          \
  RegisterGrain();                            \
  START_GRAIN(grainDescription);              \
  __VA_ARGS__                                 \
  END_GRAIN();                                \
  Success();                                  \

#define COMMON_REQUIRE_FAILURE()                              \
    PrintLocationStringFailure(__func__, __FILE__, __LINE__)  \

#define REQUIRE(description, expr)        \
  numRequirements++;                      \
  if(expr){}                              \
  else{                                   \
    RequireFail(description, #expr);      \
    COMMON_REQUIRE_FAILURE();             \
  }

#define REQUIRE_EQUAL(lhs, rhs)             \
  numRequirements++;                        \
  if( ((lhs) == (rhs)) ){}                  \
  else{                                     \
    RequireEqualFail(lhs, rhs, #lhs, #rhs); \
    COMMON_REQUIRE_FAILURE();               \
  }                                         \

namespace {
  inline bool TestRegistered = false;
  inline std::size_t numRequirements = 0;

  inline constexpr std::uint8_t width = 70;

  inline void RegisterGrain(){
    // check if there's already been a test registered
    if(TestRegistered){
      kout << "\n\nERROR: A test already has been registered, halting...";
      X8664::HaltCatchFire();
    }
    else{
      TestRegistered = true;
    }
  }

  inline void Begin(
      const char* functionName, 
      const char* fileName,
      const char* grainDescription
    )
  {
    for(std::uint8_t i = 0; i < width; i++){
      kout << '=';
    }
    kout << '\n'
         << "Beginning Grain Test" << '\n' 
         << "Name: " << functionName << "\n"
         << "File: " << fileName << '\n'
         << "Desc: " << grainDescription << '\n';
    for(std::uint8_t i = 0; i < width; i++){
      kout << '-';
    }
    kout << '\n';
  }

  inline void End(
      const char* functionName, 
      const char* fileName
    )
  {
    for(std::uint8_t i = 0; i < width; i++){
      kout << '-';
    }
    kout << '\n'
         << "End of Grain Test" << '\n' 
         << "Name: " << functionName << "\n"
         << "File: " << fileName << '\n';
    for(std::uint8_t i = 0; i < width; i++){
      kout << '=';
    }
  }

  inline void Success()
  {
    kout << intmode::dec;
    kout << "\nSuccess! " << numRequirements << " Requriements Passed! Pony?";
    PrintRarity();
    kout << "\nThe Operating System Lives! Hang around a bit? (please dont catch fire!)";
    X8664::HaltCatchFire();
  }

  inline void RequireFail(
    const char* description,
    const char* expression
  ){
    kout << "REQUIREMENT FAILURE\n";
    kout << "Assertion '" << expression << "' failed" << '\n';
    kout << "Description: " << description << '\n';
  }

  template <typename T, typename U>
  inline void RequireEqualFail(
    T lhs,
    U rhs,
    const char* lhs_text,
    const char* rhs_text
  ){
    kout << intmode::dec;
    kout << "REQUIREMENT EQUALS FAILURE\n";
    kout << "Assertion '" << lhs_text << " == " << rhs_text << "' failed" << '\n';
    // print lhs
    kout << "'" << lhs_text << "' evaluates to " << lhs << " :: " 
         << intmode::hex << "0x" << lhs << '\n';
    // print rhs
    kout << intmode::dec;
    kout << "'" << rhs_text << "' evaluates to " << rhs << " :: " 
         << intmode::hex << "0x" << rhs << '\n';
  }

  inline void PrintLocationStringFailure(
    const char* functionName, 
    const char* filename, 
    std::size_t lineNumber
  ){
    kout << "Location:\n"  << 
            filename << " :: " << 
            functionName << "() ::  "<< 
            "line " << intmode::dec << lineNumber << '\n';
    kout << "halting...";
    X8664::HaltCatchFire();
  }

}
