#ifndef KOSTREAM_HPP
#define KOSTREAM_HPP

#include <logger.hpp>

enum class intmode{dec, hex};

class kostream{
  logger serial_logger{0x3F8};
  intmode m_mode;
  public:
    // inserters
    kostream& operator<<(int i){
      serial_logger.log(i);
      return *this;
    }

    kostream& operator<<(std::uint64_t i){
      if(m_mode == intmode::hex)
        serial_logger.log_hex(i);
      else
        serial_logger.log(i);
      return *this;
    }

    kostream& operator<<(char ch){
      if(ch == '\n'){
        serial_logger.log('\r');
        serial_logger.log('\n');
      }
      else
        serial_logger.log(ch);
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
          return *this;
        case intmode::hex:
          m_mode = intmode::hex;
          return *this;
      }
    }
};

// GLOBAL VARIABLE kout - represents a serial console output stream
kostream kout;

#endif
