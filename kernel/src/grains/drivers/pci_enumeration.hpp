#include "drivers/serial/kostream.hpp"
#include "drivers/pci/pci.hpp"
#include "grains/grains.hpp"
#include "lib/kassert.hpp"

namespace Grains::Drivers{

  inline void EnumeratePCI(){
    GRAIN("enumerates the pci bus",
      {
        kout << "we trying to enumerate" << '\n';
        ::Drivers::Pci::EnumeratePCI();
        ::Drivers::Pci::CheckSataICH9();
      }
    );
  }

} // namespace Grains::Drivers
