#include "gas.hpp"

namespace Firmware::Acpi{

const char* Gas::AddressSpaceIdString(std::uint8_t id){
  switch(id){
    case 0x00:
      return "System Memory Space";
    case 0x01:
      return "System IO Space";
    case 0x02:
      return "PCI Space";
    case 0x03:
      return "Embedded Controller";
    default:
      kassert(false && "Gas::AddressSpaceIdString(std::uint8_t): unknown id");
      return "";
  }
}

void Gas::PrintValues() const{
  kout << "Address Space: " << AddressSpaceIdString(addressSpaceId) << '\n';
  kout << "Register Bit Width: " << regBitWidth << '\n';
  kout << "Register Bit Offset: " << regBitOffset << '\n';
  kout << "Address: " << address << '\n';
}

}
