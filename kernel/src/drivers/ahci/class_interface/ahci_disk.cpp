#include "ahci_disk.hpp"

namespace Drivers::Ahci{

AhciDisk::AhciDisk(std::uint8_t port)
  :
    m_port{port}
{

}

}
