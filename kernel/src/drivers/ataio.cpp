#include "ataio.hpp"
#include <cstdint>

void Disk::InitialiseAtaPio(std::uint8_t drive, std::uint8_t bus){
  if(bus == 0 && drive == 0){
    kout << "trying to initialise drive 0 on bus 0" << '\n';
  }
  else{
    kout << "ATAPIO is only supported for drive 0 and bus 0" << '\n';
    return;
  }
  outb(PrimaryDriveSelectPort, 0xa0);
  for(int i = 0; i < 15; i++){
    inb(0x1f7);			// wait 400ns for drive select to work
  }
  // setting Sectorcount, LBAlo, LBAmid, and LBAhi IO ports to 0:
  for(int i = 0x1f2; i <= 0x1f5; i++){
    outb(i, 0);
  }
  // sending identify to command port
  outb(CommandPort, 0xEC);
  // now we read
  std::uint8_t output = inb(StatusPort);
  if(output == 0){
    kout << "NONEXSISTENT_DRIVE :: drive :: " <<  drive
         << " :: bus :: " << bus << '\n';
    return;
  }
  /*
  else{
    //kout << "drive :: " << drive << " :: bus :: " << " :: exists" << '\n';
  }
  */
  // now we need to poll status port until bit 7 clears
  std::uint8_t status = inb(StatusPort);
  //std::uint32_t  LBAmid = 0x1f4;
  //std::uint32_t LBAhi = 0x1F5;
  kout << "waiting for status \n";
  while(status & 0x80){
    /*
    status = inb(StatusPort);
    kout << "status :: " << status << '\n';
    if(status & 0x08) break;
    if(status & 0x1) break;
    */
    for(int i = 0; i < 0x0FFFFFFF; i++) {}
    status = inb(StatusPort);
  }
  kout << "finished polling!" << '\n';
}
