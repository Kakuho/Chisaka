#include "identify_device_buffer.hpp"

namespace Drivers::Ahci{

IdentifyDeviceBuffer::IdentifyDeviceBuffer(){
  for(auto& i: buffer){
    i = 0;
  }
}

void IdentifyDeviceBuffer::PrintSerialNumber() const{
  for(int i = 0; i < 10; i++){
    std::uint8_t firstbyte = buffer[10+i] >> 8;
    std::uint8_t secondbyte = buffer[10+i] & 0xFF;
    kout << firstbyte << ' ' << secondbyte << ' ';
  }
  kout << '\n';
}

void IdentifyDeviceBuffer::PrintCHS() const{
  std::uint16_t cylinders = buffer[54];
  std::uint16_t heads = buffer[55];
  std::uint16_t sectors = buffer[56];

  kout << "Cylinder: " << buffer[54] 
       << ", Head: " << buffer[55] 
       << ", Sectors: " << buffer[56] 
       << '\n';

  kout << "C * H * S: " << cylinders * heads * sectors << '\n';

  std::size_t maxLba = (((cylinders * heads)+ heads) * sectors) + sectors - 1;
  kout << "Max lba: " << maxLba << '\n';

  std::uint32_t capacity = buffer[58];
  capacity <<= 16;
  capacity |= buffer[57];
  kout << "Capacity In Sectors: " << capacity << '\n';
}

void IdentifyDeviceBuffer::PrintLogicalCHS() const{

}

std::uint32_t IdentifyDeviceBuffer::NumSectors() const{
  std::uint32_t numsec = buffer[58];
  numsec <<= 16;
  numsec |= buffer[57];
  return numsec;
}

std::uint32_t IdentifyDeviceBuffer::UserAccessibleSectors() const{
  std::uint32_t dword = buffer[61];
  dword <<= 16;
  return dword | buffer[60];
}

}
