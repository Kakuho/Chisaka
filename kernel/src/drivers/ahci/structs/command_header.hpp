#pragma once

// AHCI port's Command List are comprised of Command Headers

#include <cstdint>
#include <cassert>

#include "kassert.hpp"

namespace Drivers::Ahci{

class CommandHeader{

  // Flags
  static constexpr std::uint8_t MASK_PMP = 0xF0;
  static constexpr std::uint8_t MASK_C = 0x04;
  static constexpr std::uint8_t MASK_B = 0x02;
  static constexpr std::uint8_t MASK_R = 0x01;

  static constexpr std::uint8_t MASK_P= 0x80;
  static constexpr std::uint8_t MASK_W = 0x40;
  static constexpr std::uint8_t MASK_A = 0x20;
  static constexpr std::uint8_t MASK_CFL = 0x1F;

  public:
    CommandHeader();

    CommandHeader(
        void* commandTableBase, 
        std::uint16_t prdtl, 
        std::uint8_t controlbyte1, 
        std::uint8_t controlbyte2
    );

    //-------------------------------------------------------------
    //  Attributes
    //-------------------------------------------------------------
    
    // Dword 0
    constexpr std::uint16_t GetPrdtl() const noexcept;
    constexpr void SetPrdtl(std::uint16_t newPrdtl) noexcept;

    constexpr std::uint8_t GetPortMultiplier() const noexcept;
    constexpr void SetPortMultiplier(std::uint8_t newPmp) noexcept;

    constexpr bool C() const noexcept;        // Clear Busy upon OK
    constexpr void SetC(bool c) noexcept;

    constexpr bool Bist() const noexcept;
    constexpr void SetBist(bool b) noexcept;

    constexpr bool Reset() const noexcept;
    constexpr void SetReset(bool b) noexcept;

    constexpr bool Prefetchable() const noexcept;
    constexpr void SetPrefetchable(bool b) noexcept;

    constexpr bool Write() const noexcept;
    constexpr void SetWrite(bool b) noexcept;

    constexpr bool Atapi() const noexcept;
    constexpr void SetAtapi(bool b) noexcept;

    constexpr std::uint8_t Cfl() const noexcept;
    constexpr void SetCfl(std::uint8_t val) noexcept;

    // Dword 1
    // Bytes Transferred
    constexpr std::uint32_t Prdbc() const noexcept{ return m_prdbc;}
    constexpr void SetPrdbc(std::uint32_t val) noexcept{ m_prdbc = val;}

    // Dword 2
    constexpr std::uint64_t CommandTableBase() const noexcept{
      std::uint64_t base = m_ctbau;
      return (base << 32) | m_ctba;
    }

    constexpr void SetCommandTableBase(std::uint64_t addr){
      m_ctbau = (addr >> 32);
      m_ctba = addr & 0xFFFF'FFFF;
    }

    constexpr void SetCommandTableBase(void* addr) noexcept{
      SetCommandTableBase(reinterpret_cast<std::uint64_t>(addr));
    }

  public:
    // Dword 0
    std::uint8_t m_p_w_a_cfl;
    std::uint8_t m_pmp_c_b_r;
    std::uint16_t m_prdtl;
    // Dword 1
    std::uint32_t m_prdbc;
    // Dword 2
    std::uint32_t m_ctba;
    // Dword 3
    std::uint32_t m_ctbau;
    // Dword 4
    std::uint32_t m_rsv0 = 0;
    // Dword 5
    std::uint32_t m_rsv1 = 0;
    // Dword 6
    std::uint32_t m_rsv2 = 0;
    // Dword 7
    std::uint32_t m_rsv3 = 0;
};

static_assert(sizeof(CommandHeader) == 32);

//-------------------------------------------------------------

inline CommandHeader::CommandHeader( )
  : m_p_w_a_cfl{0},
    m_pmp_c_b_r{0},
    m_prdtl{0},
    m_prdbc{0},
    m_ctba{0},
    m_ctbau{0}
{
}

inline CommandHeader::CommandHeader( 
  void* commandTableBase, 
  std::uint16_t prdtl, 
  std::uint8_t controlbyte1, 
  std::uint8_t controlbyte2
)
  :
    m_p_w_a_cfl{controlbyte2},
    m_pmp_c_b_r{controlbyte1},
    m_prdtl{prdtl},
    m_prdbc{0}
{
  kassert(!(m_pmp_c_b_r & 0b1000));
  SetCommandTableBase(commandTableBase);
}

//-------------------------------------------------------------=
//  Attributes
//-------------------------------------------------------------

constexpr std::uint16_t CommandHeader::GetPrdtl() const noexcept{
  return m_prdtl;
}

constexpr void CommandHeader::SetPrdtl(std::uint16_t newPrdtl) noexcept{
  m_prdtl = newPrdtl;
}

constexpr std::uint8_t CommandHeader::GetPortMultiplier() const noexcept{
  return m_pmp_c_b_r & 0xFFFF'0000;
}

constexpr void CommandHeader::SetPortMultiplier(std::uint8_t newPmp) noexcept{
  m_pmp_c_b_r |= (static_cast<std::uint16_t>(newPmp) << 8);
}

constexpr bool CommandHeader::C() const noexcept{
  return m_pmp_c_b_r & 0x4;
}

constexpr void CommandHeader::SetC(bool c) noexcept{
  if(c){
    m_pmp_c_b_r |= MASK_C;
  }
  else{
    m_pmp_c_b_r &= ~MASK_C;
  }
}

constexpr bool CommandHeader::Bist() const noexcept{
  return m_pmp_c_b_r & MASK_B;
}

constexpr void CommandHeader::SetBist(bool b) noexcept{
  if(b){
    m_pmp_c_b_r |= MASK_B;
  }
  else{
    m_pmp_c_b_r &= ~MASK_B;
  }
}

constexpr bool CommandHeader::Reset() const noexcept{

  return m_pmp_c_b_r & MASK_R;
}

constexpr void CommandHeader::SetReset(bool b) noexcept{
  if(b){
    m_pmp_c_b_r |= MASK_R;
  }
  else{
    m_pmp_c_b_r &= ~MASK_R;
  }
}

constexpr bool CommandHeader::Prefetchable() const noexcept{
  return m_pmp_c_b_r & MASK_P;
}

constexpr void CommandHeader::SetPrefetchable(bool b) noexcept{
  if(b){
    m_p_w_a_cfl |= MASK_P;
  }
  else{
    m_p_w_a_cfl &= ~MASK_P;
  }

}

constexpr bool CommandHeader::Write() const noexcept{
  return m_pmp_c_b_r & MASK_W;
}

constexpr void CommandHeader::SetWrite(bool b) noexcept{
  if(b){
    m_p_w_a_cfl |= MASK_W;
  }
  else{
    m_p_w_a_cfl &= ~MASK_W;
  }
}

constexpr bool CommandHeader::Atapi() const noexcept{
  return m_p_w_a_cfl & MASK_A;
}

constexpr void CommandHeader::SetAtapi(bool b) noexcept{
  if(b){
    m_p_w_a_cfl |= MASK_A;
  }
  else{
    m_p_w_a_cfl &= ~MASK_A;
  }
}

constexpr std::uint8_t CommandHeader::Cfl() const noexcept{
  return m_p_w_a_cfl & MASK_CFL;
}

constexpr void CommandHeader::SetCfl(std::uint8_t val) noexcept{
  kassert(val <= 0x10);
  m_p_w_a_cfl &= ~MASK_CFL;
  m_p_w_a_cfl |= val;
}

}
