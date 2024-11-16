#ifndef KOSTREAM_HPP
#define KOSTREAM_HPP

//#include <string>
#include <concepts>

#include "logger.hpp"

enum class intmode{dec, hex, bin};

class kostream{
  logger serial_logger{0x3F8};
  intmode m_mode;
  public:

    // inserters
    template<typename T>
      requires std::integral<T>
    kostream& operator<<(T i){
      switch(m_mode){
        case intmode::dec:
          serial_logger.log(i); 
          break;
        case intmode::hex:
          serial_logger.log_hex(i); 
          break;
        case intmode::bin:
          serial_logger.log_bin(i); 
          break;
      }
      return *this;
    }

    // it would be good to construct a member function template 
    // where we constrain type parameter to std::uintx_t
    // such that x \in {8, 16, 32, 64}

    template<typename T>
      requires std::unsigned_integral<T>
    kostream& operator<<(T i){
      switch(m_mode){
        case intmode::dec:
          serial_logger.log(i); 
          break;
        case intmode::hex:
          serial_logger.log_hex(i); 
          break;
        case intmode::bin:
          serial_logger.log_bin(i); 
          break;
      }
      return *this;
    }

    kostream& operator<<(char ch){
      if(ch == '\n'){
        serial_logger.log('\r');
        serial_logger.log('\n');
      }
      else{
        serial_logger.log(ch);
      }
      return *this;
    }

    kostream& operator<<(const char* st){
      serial_logger.log(st);
      return *this;
    }

    kostream& operator<<(intmode im){
      switch(im){
        case intmode::dec:
          m_mode = intmode::dec;
          break;
        case intmode::hex:
          m_mode = intmode::hex;
          break;
        case intmode::bin:
          m_mode = intmode::bin;
          break;
      }
      return *this;
    }
};

// GLOBAL VARIABLE kout - represents a serial console output stream
extern kostream kout;

#endif
