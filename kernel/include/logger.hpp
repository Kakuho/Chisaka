#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdint>
#include <cstdlib>

extern "C" void outb(int, char val);

class logger{
  private:
    int m_port;

  public:
    logger(int port):
      m_port{port}
    {

    }

    void log(char ch){
      outb(m_port, ch);
    }

    void log(int x){
      char buffer[100];
      int index = 0;
      if(x == 0){
        outb(m_port, 48); // ascii table, digit start at 48
        return;
      }
      else while(x > 0){
        int digit = x % 10;
        x = x / 10;
        buffer[index++] = digit + 48;
      }
      for(int i = index - 1; i >= 0; i--){
        outb(m_port, buffer[i]);
      }
    }

    void log(std::uint64_t x){
      char buffer[30];
      int index = 0;
      if(x == 0){
        outb(m_port, 48); // ascii table, digit start at 48
        return;
      }
      else while(x > 0){
        int digit = x % 10;
        x = x / 10;
        buffer[index++] = digit + 48;
      }
      for(int i = index - 1; i >= 0; i--){
        outb(m_port, buffer[i]);
      }

    }

    void log_hex(std::uint64_t x){
      char buffer[20];
      int index = 0;
      if(x == 0){
        outb(m_port, 48); // ascii table, digit start at 48
        return;
      }
      else while(x > 0){
        int digit = x % 16;
        x = x / 16;
        if(digit < 0xA){
          buffer[index++] = digit + 48;
        }
        else{ 
          // digit >= 10 = 0x0A
          buffer[index++] = digit + 55;
        }
      }
      for(int i = index - 1; i >= 0; i--){
        outb(m_port, buffer[i]);
      }
    }


    void log_bin(std::uint64_t x, int sep){
      char buffer[20];
      int index = 0;
      if(x == 0){
        outb(m_port, 48);
        return;
      }
      else while(x > 0){
        int digit = x % 2;
        x = x / 2;
        if(digit == 0){
          buffer[index++] = 48;
        }
        else{ 
          // digit >= 10 = 0x0A
          buffer[index++] = 49;
        }
      }
      for(int i = index - 1; i >= 0; i--){
        outb(m_port, buffer[i]);
      }
    }

    void log(const char* st){
      for(const char* ch = &st[0]; *ch != '\0'; ch++){
        if(*ch == '\n'){
          outb(m_port, '\r');
          outb(m_port, '\n');
        }
        else{
          outb(m_port, *ch);
        }
      }
    }
};

#endif
