#ifndef X86_64_INTERRUPTS_CONTROLLERS_PIC
#define X86_64_INTERRUPTS_CONTROLLERS_PIC

// Code for the 8259 PIC Controller
//
// Reference: C216 5.8.2

#include <cstdint>

#include "lib/kassert.hpp"

namespace X8664::Interrupt{

class PicController{

  struct CachedICW{
    std::uint16_t icw1;
    std::uint16_t icw2;
    std::uint16_t icw3;
    std::uint16_t icw4;
  };

  static constexpr inline std::uint8_t MASTER_BASE_PORT = 0x20;
  static constexpr inline std::uint8_t SLAVE_BASE_PORT = 0xa0;

  public:
    PicController(std::uint16_t masterOffset, std::uint16_t slaveOffset);
    void FlushICWs();
  
    //-------------------------------------------------------------
    // Masking
    //-------------------------------------------------------------

    [[nodiscard]] std::uint16_t Masksbm(std::uint8_t index);

    void MaskMaster(std::uint8_t index);
    void UnmaskMaster(std::uint8_t index);

    void MaskSlave(std::uint8_t index);
    void UnmaskSlave(std::uint8_t index);

  private:
    void Initialise();

  private:
    CachedICW m_masterIcws;
    CachedICW m_slaveIcws;
    const std::uint16_t m_masterOffset;
    const std::uint16_t m_slaveOffset;
};

}

#endif
