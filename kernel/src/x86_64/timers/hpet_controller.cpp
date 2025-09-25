#include "hpet_controller.hpp"

namespace X8664::Timers{

HpetController::HpetController()
  : m_regBase{nullptr},
    m_freq{0},
    m_numTimers{0}
{

}

HpetController::HpetController(Registers* base)
  : m_regBase{base},
    m_freq{0},
    m_numTimers{CalculateNumTimers()}

{
  SetLegacyReplacementRoute();
}

HpetController::HpetController(std::uint64_t base)
  : HpetController(reinterpret_cast<Registers*>(base)) {}

void HpetController::SetBaseAddress(Registers* base){
  m_regBase = base;
  m_numTimers = CalculateNumTimers();
  SetLegacyReplacementRoute();

}

void HpetController::SetBaseAddress(std::uint64_t base){
  m_regBase = reinterpret_cast<Registers*>(base);
  m_numTimers = CalculateNumTimers();
  SetLegacyReplacementRoute();
}

void HpetController::InitialiseTimer(std::uint8_t timerno){

}

std::uint64_t HpetController::CalculateFrequency() const{
  if(m_regBase){
    std::uint32_t fsPeriod = (m_regBase->gCapabilities) >> 32;
    //return 0x10000000000000000 / fsPeriod;
    // return 10^15/fsPeriod
  }
  else{
    return 0;
  }
}

std::uint8_t HpetController::CalculateNumTimers() const{
  if(m_regBase){
    return (m_regBase->gCapabilities & 0x1F00) >> 8;
  }
  else{
    return 0;
  }
}

void HpetController::SetLegacyReplacementRoute() const{
  if(LegacyRouteCapable()){
  }
  m_regBase->gConfig |= 2;
}

void HpetController::EnumerateCapabilities() const{
  const std::uint64_t cap = m_regBase->gCapabilities;
  kout << "LegacyReplacement Route Capable: " << ((cap & 0x8000) >> 15) << '\n';
  kout << "Main Counter: " << (cap & 0x200 ? "64 Bits" : "32 Bits") << '\n';
  kout << "Number of Timers: " << m_numTimers << '\n';
}

void HpetController::EnumerateTimerCapabilities(std::uint8_t timerno) const{
  kassert(timerno < m_numTimers);
  const volatile std::uint64_t& timerConfig = m_regBase->timers[timerno].config;

  kout  << "Timer " << timerno 
        << " Base address:" 
        << reinterpret_cast<std::uint64_t>(&m_regBase->timers[timerno])<< '\n';

  kout  << "Timer " << timerno 
        << " Config:" 
        << m_regBase->timers[timerno].config << '\n';

  kout  << "Timer " << timerno 
        << " Comparator:" 
        << m_regBase->timers[timerno].comparator << '\n';

  kout << '\n';

  kout  << "Timer " << timerno 
        << " Read Only Capabilities:" << '\n';

  kout  << "Timer " << timerno 
        << " Timer Size: " 
        << ((timerConfig & 0x0020) ? "64-bits" : "32-bits") << '\n';

  kout  << "Timer " << timerno 
        << " Periodic Mode Capable: " 
        << ((timerConfig & 0x0010) ? "Supported" : "Unsupported") << '\n';

  //-------------------------------------------------------------
  //
  kout  << "Timer " << timerno 
        << " Configuration:" << '\n';

  kout  << "Timer " << timerno 
        << " FSB Enable: " 
        << ((timerConfig & 0x4000) ? "True" : "False") << '\n';

  kout  << "Timer " << timerno 
        << " 32-bit mode Enable: " 
        << ((timerConfig & 0x0100) ? "True" : "False") << '\n';

  kout  << "Timer " << timerno 
        << " Periodic Mode Enabled: " 
        << ((timerConfig & 0x0040) ? "True" : "False") << '\n';

  kout  << "Timer " << timerno 
        << " Timer Type: " << (timerConfig & 0x0008) << '\n';

  kout  << "Timer " << timerno 
        << " Interrupt Enable: " 
        << ((timerConfig & 0x0004) ? "True": "False") << '\n';

  kout  << "Timer " << timerno 
        << " Interrupt Type: " 
        << ((timerConfig & 0x0002) ? "Level Trigger": "Edge Triggered") << '\n';

  kout  << "Timer " << timerno 
        << " Comparator Value: " 
        <<  m_regBase->timers[timerno].comparator << '\n';
}

void HpetController::ClearInterrupt(std::uint8_t timerno){
  m_regBase->gInterruptStatus |= (1 << timerno);
}

std::uint64_t HpetController::ReadMainCounter(){
  std::uint32_t high = m_regBase->gMainCounterHi;
  std::uint32_t low = m_regBase->gMainCounterLo;
  std::uint32_t high2 = m_regBase->gMainCounterHi;
  while(high2 != high){
    high = m_regBase->gMainCounterHi;
    low = m_regBase->gMainCounterHi;
    high2 = m_regBase->gMainCounterHi;
  }
  std::uint64_t count = high2;
  count <<= 32;
  count |= low;
  return count;
}

void HpetController::SetMainCounter(std::uint64_t val){
  while(m_regBase->gConfig & 1){
    continue;
  }
  m_regBase->gMainCounterLo = val&0xFFFF;
  m_regBase->gMainCounterHi = val>>32;
}

} 
