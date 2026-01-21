#pragma once

#include <cstdint>
#include <iomanip>
#include <fstream>
#include <cstring>

namespace Mocks{
  struct Page{

    Page(){
      memset(bytes, 0, PAGE_SIZE);
    }

    static constexpr std::size_t PAGE_SIZE = 0x1000;

    std::uint8_t bytes[0x1000];

    void Dump(const char* filename){
      std::ofstream ost{filename};
      ost << std::setbase(16) << std::setw(2);
      ost << "Page base: " << reinterpret_cast<std::uintptr_t>(&bytes[0]) << '\n';
      for(std::size_t i = 1; i < PAGE_SIZE+1; i++){
        ost << std::setfill('0') << std::setw(2) << ((unsigned)bytes[i-1] & 0xFF) << ' ';
        if(i % 32 == 0){
          ost << '\n';
        }
      }
    }
  };
};
