#pragma once

#include "../mfs_manager.hpp"

#include "aii/array.hpp"
#include "drivers/ahci/class_interface/ahci_driver.hpp"
#include "memory/heap/allocator.hpp"

namespace Fs::Merofs::Samples{
  inline void PrintBuffer(std::uint8_t* buffer, std::size_t count){
    for(std::size_t i = 0; i < count; i++){
      kout << buffer[i] << ' ';
      if((i+1)%32 == 0) kout << '\n';
    }
  }

  struct Buffer{
    Aii::Array<std::uint8_t, 0x90> buffer;
  };

  inline void Hehe(){
    // trying to isolate the problem in Initialise
    std::uint8_t* hehe = reinterpret_cast<std::uint8_t*>(0x0000000000000040);
    *hehe = 2;

    Mem::PageAllocator::Initialise();
    Mem::Heap::Allocator::Initialise();
    for(std::size_t i = 0; i < 0x700; i++){
      auto* yaya = Mem::Heap::Allocator::New<Buffer>();
      kout << "Allocated " << i << " buffer\n";
      Mem::Heap::Allocator::Delete(yaya);
    }
    kout << "el fin\n";
  }

  inline void Initialise(){

    // first gotta init the ahci driver
    Mem::PageAllocator::Initialise();
    Mem::Heap::Allocator::Initialise();
    auto& ahcidriver = Drivers::Ahci::AhciDriver::Get();
    ahcidriver.Init();

    // kernel buffer for transactions
    auto* databuffer = reinterpret_cast<std::uint8_t*>(Mem::Heap::Allocator::Allocate(512));
    Aii::Memset(databuffer, 0, 512);

    // now we can init our filesystem
    MfsManager mfs{ahcidriver, 0};
    ahcidriver.ReadSectorPolled(0, 0, databuffer);
    PrintBuffer(databuffer, 512);

    auto& disk = ahcidriver.Disk(0);
    kout << "Disk Total Sectors: " << disk.TotalSectors() << '\n';

  }
}
