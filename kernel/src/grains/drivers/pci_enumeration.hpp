#include "drivers/serial/kostream.hpp"
#include "drivers/pci/pci.hpp"
#include "grains/grains.hpp"
#include "lib/kassert.hpp"

namespace Grains::Drivers{

  inline void EnumeratePCI(){
    GRAIN("enumerates the pci bus",
      {
        ::Drivers::Pci::EnumeratePCI();
      }
    );
  }

} // namespace Grains::Drivers
