#include "ahci_disk_polled.hpp"
#include "ahci_driver.hpp"

namespace Drivers::Ahci{

AhciDiskPolled::AhciDiskPolled(std::uint8_t port)
  :
    m_port{port}
{

}

void AhciDiskPolled::Read(std::uint16_t sector, std::uint8_t* buffer){
  AhciDriver::Get().ReadSector(m_port, sector, buffer);
  AhciDriver::Get().WaitForPortInterrupt(m_port);
}

void AhciDiskPolled::Write(std::uint16_t sector, std::uint8_t* buffer){
  AhciDriver::Get().WriteSector(m_port, sector, buffer);
  AhciDriver::Get().WaitForPortInterrupt(m_port);
}

}
