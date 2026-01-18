#include "apic.hpp"
#include "x86_64/features/features.hpp"

#include "types.hpp"

namespace X8664::Interrupts{

ApicController::ApicController()
  :
    m_enabled{false},
    m_present{Features::SupportsAPIC()},
    m_x2apicSupport{Features::Supportsx2APiC()}
{
  if(!m_present){
    kout << "Apic Controller not detected..." << '\n';
    return;
  }
  m_base = Features::GetApicBase();
  InitialiseLvt();
}

void ApicController::EnableController(){
  // First set the msr
  Features::EnableApic();
  // Now set the spurious vector
  std::uint32_t svr = ReadSpuriousVectorRegister();
  WriteSpuriousVectorRegister(svr | 0x100);
  // For now disable x2apic
  Features::Disablex2Apic();
  m_enabled = true;
}

void ApicController::DisableController(){
  //  Set the MSR
  Features::DisableApic();
  //  Now set the SVR
  std::uint32_t svr = ReadSpuriousVectorRegister();
  WriteSpuriousVectorRegister(svr & ~0x100);
  // For now disable x2apic
  Features::Disablex2Apic();
  m_enabled = false;
}

void ApicController::Enumerate(){
  // Check the features of the Apic Controller
  if(m_present){
    kout << "On die APIC Present" << '\n';
    kout << (m_x2apicSupport ? 
             "x2Apic Supported" : 
             "x2Apic not Supported") 
         << '\n';
    kout << (SupportsEoiSupression() ? 
             "EOI Supression Supported" : 
             "EOI Supression not supported") 
         << '\n';
  }
  else{
    kout << "On die APIC is not present" << '\n';
  }

}

bool ApicController::IsPresent(){
  if(Features::SupportsAPIC()){
    return true;
  }
  else{
    return false;
  }
  return m_present;
}

bool ApicController::Supportsx2Apic(){
  if(Features::Supportsx2APiC()){
    m_x2apicSupport = true;
  }
  else{
    m_x2apicSupport = false;
  }
  return m_x2apicSupport;
}

bool ApicController::SupportsEoiSupression(){
  std::uint32_t vers = ReadVersion();
  return (vers >> 24) & 1;
}

void ApicController::WriteEoi(std::uint32_t val){
  *(reinterpret_cast<std::uint32_t*>(m_base + 0xb0)) = val;
}
    
void ApicController::SignalEoi(){
  WriteEoi(0);
}

void ApicController::BlockFlatInterrupts(){
  WriteTpr(0xFF);
}

void ApicController::EnableFlatInterrupts(){
  WriteTpr(0x00);
}

void ApicController::SetTaskPriority(std::uint8_t val){
  WriteTpr(val);
}

void ApicController::SetSpuriousVector(std::uint8_t val){
  std::uint32_t svr = ReadSpuriousVectorRegister() & ~0xFF;
  WriteSpuriousVectorRegister(svr | val);
}

// Registers 

Chisaka::PhysAddr64 ApicController::GetBaseAddress() const{
  return m_base;
}

std::uint32_t ApicController::ReadId() const{
  return *(reinterpret_cast<std::uint32_t*>(m_base + 0x20));
}

std::uint32_t ApicController::ReadVersion() const{
  return *(reinterpret_cast<std::uint32_t*>(m_base + 0x30));
}

void ApicController::EnumerateVersion() const{
  std::uint32_t vers = ReadVersion();
  kout << "Enumerating Apic Version Register..." << '\n';
  // Print the APIC Version
  kout << "Max LVT Entries = " << (vers >> 16) + 1 << '\n';
  kout << "Supressible EOI Broadcasts: " << ((vers >> 24) == true) << '\n';
}

std::uint32_t ApicController::ReadTpr() const{
  return *(reinterpret_cast<std::uint32_t*>(m_base + 0x80));
}

void ApicController::WriteTpr(std::uint32_t val){
  *reinterpret_cast<std::uint32_t*>(m_base + 0x80) = val;
}

std::uint32_t ApicController::ReadPpr() const{
  return *(reinterpret_cast<std::uint32_t*>(m_base + 0xa0));
}

std::uint32_t ApicController::ReadDfr() const{
  return *(reinterpret_cast<std::uint32_t*>(m_base + 0xe0));
}

std::uint32_t ApicController::ReadSpuriousVectorRegister() const{
  return *(reinterpret_cast<std::uint32_t*>(m_base + 0xf0));
}

void ApicController::WriteSpuriousVectorRegister(std::uint32_t val){
  *reinterpret_cast<std::uint32_t*>(m_base + 0xf0) = val;
}

// Local Vector Table

void ApicController::InitialiseLvt(){
  // Just an example of initialising LINT0 and LINT1, since these are what you like
  // Lint 0 - interrupt vector 17
  // 0x11
  WriteLint0(0b0'101'0'000'00010001);  
  // Lint 1 - interrupt vector 18
  // 0x12
  WriteLint1(0b0'101'0'000'00010010);  
}

void ApicController::EnumerateLvt() const{
  kout << "Enumerating Local Vector Table..." << '\n';
  kout << "Enumerating Lint0..." << '\n';
  EnumerateLvtEntry(ReadLint0());
  kout << "Enumerating Lint1..." << '\n';
  EnumerateLvtEntry(ReadLint1());
  kout << "Enumerating Error..." << '\n';
  EnumerateLvtEntry(ReadError());
  kout << "Finished Local Vector Table Enumeration" << '\n';
}

void ApicController::EnumerateLvtEntry(std::uint32_t lvtEntry) const{
  kout << "Vector: " << (lvtEntry & 0xFF) << '\n';
  kout << "Delivery Mode: " << (lvtEntry & 0x700) << '\n';
  kout << "Delivery Pending: " << ((lvtEntry >> 12) & 1) << '\n';
  kout << "Interrupt Pin Polarity: " << ((lvtEntry >> 13) & 1) << '\n';
  kout << "Remote IRR: " << ((lvtEntry >> 14) & 1) << '\n';
  kout << "Trigger Mode: " << ((lvtEntry >> 15) & 1) << '\n';
  kout << "Masked: " << ((lvtEntry >> 16) & 1) << '\n';
}

std::uint32_t ApicController::ReadLint0() const{
  return *(reinterpret_cast<std::uint32_t*>(m_base + LVT_LINT0_OFFSET));
}

void ApicController::WriteLint0(std::uint32_t val){
  *reinterpret_cast<std::uint32_t*>(m_base + LVT_LINT0_OFFSET) = val;
}

void ApicController::MaskLint0(){
  WriteLint0(ReadLint0() | 0x10000);
}

std::uint32_t ApicController::ReadLint1() const{
  return *(reinterpret_cast<std::uint32_t*>(m_base + LVT_LINT1_OFFSET));
}

void ApicController::WriteLint1(std::uint32_t val){
  *reinterpret_cast<std::uint32_t*>(m_base + LVT_LINT1_OFFSET) = val;
}

std::uint32_t ApicController::ReadError() const{
  return *(reinterpret_cast<std::uint32_t*>(m_base + LVT_ERROR_OFFSET));
}

void ApicController::WriteError(std::uint32_t val){
  *reinterpret_cast<std::uint32_t*>(m_base + LVT_ERROR_OFFSET) = val;
}

} // namespace X8664::Interrupts
