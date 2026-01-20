#include "ahci_disk.hpp"
#include "ahci_driver.hpp"

namespace Chisaka::Ahci{

AhciDisk::AhciDisk(std::uint8_t port)
  :
    m_port{port}
{

}

void AhciDisk::ReadPolled(std::uint16_t sector, std::uint8_t* buffer){
  AhciDriver::Get().ReadSector(m_port, sector, buffer);
  AhciDriver::Get().WaitForPortInterrupt(m_port);
}

void AhciDisk::WritePolled(std::uint16_t sector, std::uint8_t* buffer){
  AhciDriver::Get().WriteSector(m_port, sector, buffer);
  AhciDriver::Get().WaitForPortInterrupt(m_port);
}

}
