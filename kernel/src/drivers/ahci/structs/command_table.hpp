#pragma once

// AHCI port's Command Headers points to a Command Table

#include <cstdint>
#include <cassert>

#include "prdt_entry.hpp"
#include "drivers/sata/fis/h2d_register.hpp"
#include "lib/string.h"
#include "lib/kassert.hpp"
#include "aii/array.hpp"

namespace Drivers::Ahci{

static constexpr std::size_t PRDT_SLOTS = 1;

class CommandTable{
  public:
    CommandTable();

    void SetCommandFis(Sata::Fis::H2DRegister::Frame& d2hrFis);
    void SetPrdtEntry(std::uint16_t entry, PrdtEntry prdt);

    std::uint8_t* CfisBase(){return &m_cfis[0];};
    std::uint8_t* AcmdBase(){return &m_acmd[0];};
    std::uint8_t* RsvBase() {return &m_rsv[0];};
    std::uint8_t* PrdtBase(){return reinterpret_cast<std::uint8_t*>(&m_prdt[0]);};

  public:
    Aii::Array<std::uint8_t, 64> m_cfis; 
    Aii::Array<std::uint8_t, 16> m_acmd; 
    Aii::Array<std::uint8_t, 48> m_rsv;
    Aii::Array<PrdtEntry, PRDT_SLOTS> m_prdt;

    static_assert(sizeof(m_cfis) == 64);
    static_assert(sizeof(m_acmd) == 16);
    static_assert(sizeof(m_rsv) == 48);
    static_assert(sizeof(m_prdt) == sizeof(PrdtEntry)*PRDT_SLOTS);
};

static_assert(sizeof(CommandTable) == 64 + 16 + 48 + sizeof(PrdtEntry)*PRDT_SLOTS);

//-------------------------------------------------------------

inline CommandTable::CommandTable(){
  for(std::size_t i = 0; i < m_cfis.Size(); i++){
    m_cfis[i] = 0;
  }
  for(std::size_t i = 0; i < m_acmd.Size(); i++){
    m_acmd[i] = 0;
  }
  for(std::size_t i = 0; i < m_rsv.Size(); i++){
    m_rsv[i] = 0;
  }
}

inline void CommandTable::SetCommandFis(Sata::Fis::H2DRegister::Frame& d2hrFis){
  memcpy(&m_cfis, &d2hrFis, sizeof(Sata::Fis::H2DRegister::Frame));
}

inline void CommandTable::SetPrdtEntry(std::uint16_t entry, PrdtEntry prdt){
  kassert(entry < PRDT_SLOTS);
  m_prdt[entry] = prdt;
}

}
