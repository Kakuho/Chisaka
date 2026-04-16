#ifndef KOSTREAM_HPP
#define KOSTREAM_HPP

//#include <string>
#include <concepts>

enum class intmode{dec, hex, bin};

#ifdef USERMODE_TESTING

// in usermode testing kostream is just a wrapper around std::cout

#include <ios>
#include <iostream>

class kostream{
  intmode m_mode;
  public:
    // inserters
    template<typename T>
      requires std::integral<T>
    kostream& operator<<(T i){
      switch(m_mode){
        case intmode::dec:
          std::cout <<  std::dec << i;
          break;
        case intmode::hex:
          std::cout <<  std::hex << i;
          break;
        case intmode::bin:
          std::cout << i;
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
          std::cout <<  std::dec << i;
          break;
        case intmode::hex:
          std::cout <<  std::hex << i;
          break;
        case intmode::bin:
          std::cout << i;
          break;
      }

      return *this;
    }

    kostream& operator<<(char ch){
      std::cout << ch;
      return *this;
    }

    kostream& operator<<(const char* st){
      std::cout << st;
      return *this;
    }

    kostream& operator<<(intmode im){
      m_mode = im;
      return *this;
    }
};

#else

// 

#include "logger.hpp"

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

#endif

// GLOBAL VARIABLE kout - represents a serial console output stream
extern kostream kout;

#endif
