#include "ahci_driver.hpp"
#include <cstdint>

namespace Drivers::Ahci{

AhciDriver::AhciDriver()
  :
    m_present{}
{
  Initialise();
}

void AhciDriver::Initialise(){
  InitPresent();
  if(!Present()){
    kout << "Ahci driver is not present...\n";
  }
  kout <<"Initialising Ahci...\n";
  InitAbar();
  InitMMIOBase();
  InitPorts();
  InitMemory();
  Enable();
  ResetController();
  if(!HbaIdle()){
    kout << "Hba is not idle... forcing to idle...\n";
    ForceHbaIdle();
  }
  kout << "Hba is idle... \n";
  EnablePortsFRE();
  ClearPortsSERR();
  EnableInterrupts();
}

void AhciDriver::InitAbar(){
  kassert(Present());
  Pci::ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  m_abar = abar;
}

void AhciDriver::InitMMIOBase(){
  kassert(Present());
  Pci::ioaddr32_t abarOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;
  m_mmioBase = base;
  SetHostRegisters(reinterpret_cast<HostRegisters*>(m_mmioBase));
}

void AhciDriver::InitPresent(){
  Pci::ioaddr32_t subBaseClassOffset = Pci::FormConfigAddress(1, 0, 31, 2, 0x08);
  outl(Pci::CONFIG_ADDRESS, subBaseClassOffset);
  const std::uint32_t classCode = inl(Pci::CONFIG_DATA) >> 8;
  const std::uint8_t baseClass = (classCode >> 16) & 0xFF;
  const std::uint8_t subClass = (classCode >> 8) & 0xFF;
  const std::uint8_t programmingInterface = classCode & 0xFF;
  kassert(baseClass == 0x1);
  kassert(subClass == 0x06);
  kassert(programmingInterface == 0x01);
  if((baseClass != 0x1) && (subClass != 0x06) && (programmingInterface != 0x01)){
    m_present = true;
  }
  else{
    m_present = false;
  }
}

void AhciDriver::EnumerateImplementedPorts(){
  kout << "Enumerating Implemented Ports..." << '\n';
  const std::uint32_t piVal = PortsImplemented();
  kout << "HBA.PI = " << piVal << '\n';
  for(std::uint8_t i = 0; i < 32; i++){
    if(piVal & (1 << i)){
      kout << "port " << i << " is implemented\n";
    }
  }
}

void AhciDriver::InitPorts(){
  kout << "Initialising Ports..." << '\n';
  const std::uint32_t piVal = PortsImplemented();
  for(std::uint8_t i = 0; i < 32; i++){
    if(piVal & (1 << i)){
      std::uint32_t portmmiobase = MMIOBase() + 0x100 + (0x80*i);
      m_ports[i].SetMMIOBase(reinterpret_cast<AhciPort::PortRegisters*>(portmmiobase));
      m_ports[i].SetPresent();
    }
  }
}

void AhciDriver::InitMemory(){
  kassert(AhciEnabled());
  for(std::uint8_t i = 0; i < 32; i++){
    if(Port(i).Present()){
      kout << "Setting up Memory for Port " << i << '\n';
      void* portCommandBase = Kernel::kmalloc(sizeof(CommandList));
      kassert(reinterpret_cast<std::uintptr_t>(portCommandBase) % 0x400 == 0);
      Port(i).SetCLB(reinterpret_cast<std::uintptr_t>(portCommandBase));

      void* portFisBase = Kernel::kmalloc(sizeof(RecievedFis));
      kassert(reinterpret_cast<std::uintptr_t>(portFisBase) % 0x100 == 0);
      Port(i).SetCLB(reinterpret_cast<std::uintptr_t>(portFisBase));
      kout << "Finished Setting up Memory for Port " << i << '\n';
    }
  }
}

void AhciDriver::ClearIs(std::uint32_t index){
  m_hostregs->is &= ~(1 << index);
}

void AhciDriver::Enable(){
  kout << "Enabling Ahci Mode (GHC.AE)..." << '\n';
  kassert(Present());
  if(Ghc() & 0x8000'0000){
    kout << "Ahci Mode already enabled" << '\n';
  }
  else{
    kout << "Ahci Mode is not enabled" << '\n';
    SetGhc(Ghc() | 0x8000'0000);
    while(!(Ghc() & 0x8000'0000)){
      ;
    }
    kout << "Ahci Mode Enabled" << '\n';
  }
  AhciEnabled() = true;
  return;
}

void AhciDriver::EnablePortsFRE(){
  // set the ports fis recieved enable bit
  for(std::uint32_t i = 0; i < MAX_PORTS; i++){
    if(Port(i).Present()){
      Port(i).EnableFRE();
    }
  }
}

void AhciDriver::DisablePortsFRE(){
  // set the ports fis recieved enable bit
  for(std::uint32_t i = 0; i < MAX_PORTS; i++){
    if(Port(i).Present()){
      Port(i).DisableFRE();
    }
  }
}

void AhciDriver::EnableInterrupts(){

}

void AhciDriver::ResetController(){
  kout << "Resetting the Ahci Controller..." << '\n';
  kassert(Present());
  SetGhc(Ghc() | 1);
  while(Ghc() & 1){
    ;;
  }
  kout << "Finished Resetting the Ahci Controller" << '\n';
}

bool AhciDriver::HbaIdle(){
  kout << "Checking If HBA is Idle..." << '\n';
  for(std::uint8_t i = 0; i < 32; i++){
    if(Port(i).Present()){
      if(!Port(i).Idle()){
        kout << "Port " << i << " is not Idle..." << '\n';
        return false;
      }
    }
  }
  return true;
}

void AhciDriver::ForceHbaIdle(){
  kassert(AhciEnabled());
  for(std::uint8_t i = 0; i < MAX_PORTS; i++){
    if(Port(i).Present()){
      if(Port(i).Idle()){
        kout << "Port " << i << " is idle" << '\n';
      }
      else{
        kout << "Port " << i << " is not idle, forcing idle..." << '\n';
        Port(i).ForceIdle();
        kout << "Port " << i << " is now idle" << '\n';
      }
    }

  }
}

void AhciDriver::ClearPortsSERR(){
  for(std::uint8_t i = 0; i < MAX_PORTS; i++){
    if(Port(i).Present()){
      Port(i).ClearSERR();
    }
  }
}

}
