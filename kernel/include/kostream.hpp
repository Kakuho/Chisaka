#ifndef KOSTREAM_HPP
#define KOSTREAM_HPP

#include <logger.hpp>

class kostream{
  logger serial_logger{0x3F8};
  public:
    // inserters
    kostream& operator<<(int i){
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
};

// GLOBAL VARIABLE kout - represents a serial console output stream
kostream kout;

#endif
