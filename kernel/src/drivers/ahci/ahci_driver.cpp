#include "ahci_driver.hpp"

namespace Chisaka::Ahci{

namespace{
  AhciDriver ahciDriver;
}

AhciDriver::AhciDriver()
  :
    m_present{}
{
}

AhciDriver& AhciDriver::Get(){
  return ahciDriver;
}

void AhciDriver::Initialise(){
  InitPresent();
  if(!Present()){
    kout << intmode::hex << "Ahci driver is not present...\n";
  }
  kout <<"Initialising Ahci...\n";
  InitAbar();
  InitMMIOBase();
  Enable();
  ResetController();
  kout << "HBA Command Slots: " << HBACommandSlots() << '\n';
  if(!HbaIdle()){
    kout << "Hba is not idle... forcing to idle...\n";
    ForceHbaIdle();
  }
  kout << "Hba is idle... \n";
  InitPorts();
  InitMemory();
  EnablePortsFRE();
  ClearPortsSERR();
  SetPortDevices();
  EnableInterrupts();
  StartDMAEngines();
  InitDisks();
}

void AhciDriver::InitPresent(){
  Drivers::Pci::ioaddr32_t subBaseClassOffset = Drivers::Pci::FormConfigAddress(1, 0, 31, 2, 0x08);
  outl(Drivers::Pci::CONFIG_ADDRESS, subBaseClassOffset);
  const std::uint32_t classCode = inl(Drivers::Pci::CONFIG_DATA) >> 8;
  const std::uint8_t baseClass = (classCode >> 16) & 0xFF;
  const std::uint8_t subClass = (classCode >> 8) & 0xFF;
  const std::uint8_t programmingInterface = classCode & 0xFF;
  kassert(baseClass == 0x1);
  kassert(subClass == 0x06);
  kassert(programmingInterface == 0x01);
  // pci spec for the values
  if((baseClass == 0x1) && (subClass == 0x06) && (programmingInterface == 0x01)){
    m_present = true;
  }
  else{
    m_present = false;
  }
}

void AhciDriver::InitAbar(){
  kassert(Present());
  Drivers::Pci::ioaddr32_t abarOffset = Drivers::Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Drivers::Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Drivers::Pci::CONFIG_DATA);
  kout << intmode::hex << "abar: " << abar << '\n';
  m_abar = abar;
}

void AhciDriver::InitMMIOBase(){
  kassert(Present());
  Drivers::Pci::ioaddr32_t abarOffset = Drivers::Pci::FormConfigAddress(1, 0, 31, 2, 0x24);
  outl(Drivers::Pci::CONFIG_ADDRESS, abarOffset);
  const std::uint32_t abar = inl(Drivers::Pci::CONFIG_DATA);
  const std::uint32_t base = (abar >> 8) << 8;
  kout << intmode::hex << "hba mmio base: " << base << '\n';
  m_mmioBase = base;
  SetHostRegisters(reinterpret_cast<HostRegisters*>(m_mmioBase));
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
      kout << "Port " << i << " Present\n";
      std::uint32_t portmmiobase = MMIOBase() + 0x100 + (0x80*i);
      m_ports[i].SetMMIOBase(reinterpret_cast<AhciPort::PortRegisters*>(portmmiobase));
      m_ports[i].SetPresent();
    }
  }
}

void AhciDriver::SetPortDevices(){
  kassert(AhciEnabled());
  for(std::uint8_t i = 0; i < MAX_PORTS; i++){
    if(Port(i).Present()){
      if(Port(i).HasDevice()){
        kout << "Port " << i << "Has Device\n";
        Port(i).SetDevicePresent();
      }
      else{
        kout << "Port " << i << "Has no device\n";
      }
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

      kout << "Port " << i 
           << " Command List Base: " << reinterpret_cast<std::uintptr_t>(portCommandBase) 
           << '\n'
           << "Fis Base: " << reinterpret_cast<std::uintptr_t>(portFisBase) << '\n';
          
    }
  }
}

void AhciDriver::InitDisks(){
  for(std::uint8_t i = 0; i < MAX_PORTS; i++){
    if(Port(i).DevicePresent()){
      kout << "Initialising Disk " << i << '\n';
      SetupDisk(i);
    }
  }
}

void AhciDriver::SetupDisk(std::uint8_t port){
  m_disks[port] = AhciDisk{port};
  // perform identify device, and then set its total sectors
  auto* identbuffer = KContext::KHeap::Get().New<IdentifyDeviceBuffer>();
  IdentifyDevice(port, identbuffer);
  m_disks[port].SetTotalSectors(identbuffer->NumSectors());
  kout << "Port " << port << " has " << m_disks[port].TotalSectors() << '\n';
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

// Interrupts

void AhciDriver::ClearIs(){
  m_hostregs->is = 1;
}

void AhciDriver::ClearIs(std::uint32_t index){
  m_hostregs->is |= (1 << index);
}

void AhciDriver::ClearHBAInterruptStatus(){
  kassert(AhciEnabled());
  ClearIs();
  kout << "Hba IS Cleared" << '\n';
}

void AhciDriver::EnableHBAInterrupts(){
  kassert(AhciEnabled());
  SetGhc(Ghc() | 0b10);
  kout << "Host Interrupts Enabled" << '\n';
}

void AhciDriver::DisableHBAInterrupts(){
  kassert(AhciEnabled());
  SetGhc(Ghc() & ~0b10);
  kout << "Host Interrupts Disabled" << '\n';
}

void AhciDriver::ClearPortsInterruptStatus(){
  kassert(AhciEnabled());
  for(std::uint8_t i = 0; i < MAX_PORTS; i++){
    if(Port(i).Present()){
      Port(i).ClearInterruptStatus();
    }
  }
}

void AhciDriver::EnablePortsInterrupts(){
  kassert(AhciEnabled());
  for(std::uint8_t i = 0; i < MAX_PORTS; i++){
    if(Port(i).Present()){
      Port(i).EnableInterrupts();
    }
  }
}

void AhciDriver::DisablePortsInterrupts(){
  kassert(AhciEnabled());
  for(std::uint8_t i = 0; i < MAX_PORTS; i++){
    if(Port(i).Present()){
      Port(i).DisableInterrupts();
    }
  }
}

void AhciDriver::EnableInterrupts(){
  ClearPortsInterruptStatus();
  ClearHBAInterruptStatus();
  EnablePortsInterrupts();
  EnableHBAInterrupts();
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

std::uint32_t AhciDriver::HBACommandSlots() const{
  volatile std::uint32_t cap = Capabilites();
  return cap & NCS >> 8;
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
  kassert(AhciEnabled());
  for(std::uint8_t i = 0; i < MAX_PORTS; i++){
    if(Port(i).Present()){
      Port(i).ClearSERR();
    }
  }
}

void AhciDriver::StartDMAEngines(){
  kassert(AhciEnabled());
  for(std::uint8_t i = 0; i < 32; i++){
    if(Port(i).Present() && Port(i).DevicePresent()){
      Port(i).EnableDMAEngine();
      if(Port(i).DmaRunning()){
        kout << "Port " << i << " DMA Engine Started" << '\n';
      }
      else{
        kout << "Failed to start Port " << i << " DMA Engine" << '\n';
      }
    }
  }
}

void AhciDriver::PrintPortDevicePresent(){
  kassert(AhciEnabled());
  for(std::uint8_t i = 0; i < MAX_PORTS; i++){
    if(Port(i).Present() && Port(i).DevicePresent()){
      kout << "Port " << i << " has device present" << '\n';
    }
  }
}

void AhciDriver::PrintCapabilities(){
  kassert(AhciEnabled());
  // For now im just gonna get the obvious ones,
  // later i will do complete enumeration
  const volatile std::uint32_t cap = Capabilites();
  kout << "Cap: " << cap << '\n';
  kout << "Enumerating Capabilities" << '\n';
  // Bit flags
  if(cap & CAP_FLAGS::S64A){
    kout << "Supports 64-bit Addressing" << '\n';
  }
  if(cap & CAP_FLAGS::SNCQ){
    kout << "Supports Native Command Queueing" << '\n';
  }
  if(cap & CAP_FLAGS::SSNTF){
    kout << "Supports SNotification Register" << '\n';
  }
  if(cap & CAP_FLAGS::SAM){
    kout << "Supports AHCI Mode Only" << '\n';
  }
  // Quantities
  kout << "Number of ports: " << (cap & 0x1F) << '\n'
       << "Number of Command Slots: " << ((cap & NCS) >> 8) << '\n';
}

void AhciDriver::WaitForPortInterrupt(std::uint8_t port){
  //kout << "Is: " << Is() << '\n';
  //kout << "Port IS: " << Port(port).IS() << '\n';

  while(!(Is() & (1 << port))){
  }
  ClearIs(port);
  Port(port).ClearInterruptStatus();

  //kout << "Is: " << Is() << '\n';
  //kout << "Port IS: " << Port(port).IS() << '\n';
}

// Disk operations

void AhciDriver::Perform(
    std::uint8_t port, 
    CommandHeader& cmdheader,
    CommandTable* cmdtbl
){
  // Low level operation, performs the operation as defined in the commandtable
  // Command table provides the h2d fis

  int freeSlot = Port(port).EmptyCommandSlot();
  auto cmdlist = Port(port).CommandListPtr();
  cmdlist->m_headers[freeSlot] = cmdheader;
  Port(port).IssueCommand(freeSlot);

  // Now we need to wait for the command to finish
  while(Port(port).CommandSlotSet(freeSlot)){
    // spin lol
    ;;
  }
}

void AhciDriver::IdentifyDevice(
    std::uint8_t port, 
    std::uint8_t* buffer
){
  auto identFis = H2dRegisterFis::CreateIdentifyDevice();
  // create the command table
  auto* cmdtable = KContext::KHeap::Get().New<CommandTable>();
  cmdtable->SetCommandFis(identFis);
  cmdtable->SetPrdtEntry(0,
      PrdtEntry{reinterpret_cast<void*>(buffer), true, 511}
  );

  // set the new command header
  auto cmdheader = CommandHeader{
    cmdtable,
    1,
    0b0000'0'100,
    0b100'00101,
  };

  // now perform the logic to read / write
  int freeSlot = Port(port).EmptyCommandSlot();
  auto cmdlist = Port(port).CommandListPtr();
  cmdlist->m_headers[freeSlot] = cmdheader;
  Port(port).IssueCommand(freeSlot);

  // Now we need to wait for the command to finish
  while(Port(port).CommandSlotSet(freeSlot)){
    // spin lol
    ;;
  }

  KContext::KHeap::Get().Delete(cmdtable);
}

void AhciDriver::WriteSector(
    std::uint8_t port, 
    std::uint64_t sector, 
    std::uint8_t* ibuffer
){
  auto writeFis = H2dRegisterFis::CreateWrite(sector);
  // create the command table
  auto cmdtable = KContext::KHeap::Get().New<CommandTable>();
  cmdtable->SetCommandFis(writeFis);
  cmdtable->SetPrdtEntry(0,
      PrdtEntry{reinterpret_cast<void*>(ibuffer), true, 511}
  );

  // set the new command header
  auto cmdheader = CommandHeader{cmdtable,
    1,
    0b0000'0'100,
    0b010'00101,
  };

  // now perform the logic to read / write
  int freeSlot = Port(port).EmptyCommandSlot();
  auto cmdlist = Port(port).CommandListPtr();
  cmdlist->m_headers[freeSlot] = cmdheader;
  Port(port).IssueCommand(freeSlot);

  // Now we need to wait for the command to finish
  while(Port(port).CommandSlotSet(freeSlot)){
    // spin lol
    ;;
  }

  KContext::KHeap::Get().Delete(cmdtable);
}

void AhciDriver::WriteSector(
    std::uint8_t port, 
    std::uint64_t sector, 
    std::uint8_t* ibuffer,
    CommandTable* cmdtable
){
  auto writeFis = H2dRegisterFis::CreateWrite(sector);
  // create the command table
  cmdtable->SetCommandFis(writeFis);
  cmdtable->SetPrdtEntry(0,
      PrdtEntry{reinterpret_cast<void*>(ibuffer), true, 511}
  );

  // set the new command header
  auto cmdheader = CommandHeader{cmdtable,
    1,
    0b0000'0'100,
    0b010'00101,
  };

  // now perform the logic to read / write
  int freeSlot = Port(port).EmptyCommandSlot();
  auto cmdlist = Port(port).CommandListPtr();
  cmdlist->m_headers[freeSlot] = cmdheader;
  Port(port).IssueCommand(freeSlot);

  // Now we need to wait for the command to finish
  while(Port(port).CommandSlotSet(freeSlot)){
    // spin lol
    ;;
  }
}

void AhciDriver::ReadSector(
    std::uint8_t port, 
    std::uint64_t sector, 
    std::uint8_t* ibuffer
){
  auto readFis = H2dRegisterFis::CreateRead(sector);
  // create the command table
  auto cmdtable = KContext::KHeap::Get().New<CommandTable>();
  cmdtable->SetCommandFis(readFis);
  cmdtable->SetPrdtEntry(0,
      PrdtEntry{reinterpret_cast<void*>(ibuffer), true, 511}
  );

  // set the new command header
  auto cmdheader = CommandHeader{cmdtable,
    1,
    0b0000'0'100,
    0b010'00101,
  };

  // now perform the logic to read / write
  int freeSlot = Port(port).EmptyCommandSlot();
  auto cmdlist = Port(port).CommandListPtr();
  cmdlist->m_headers[freeSlot] = cmdheader;
  Port(port).IssueCommand(freeSlot);

  // Now we need to wait for the command to finish
  while(Port(port).CommandSlotSet(freeSlot)){
    // spin lol
    ;;
  }

  KContext::KHeap::Get().Delete(cmdtable);
}

void AhciDriver::ReadSector(
    std::uint8_t port, 
    std::uint64_t sector, 
    std::uint8_t* ibuffer,
    CommandTable* cmdtable
){
  auto readFis = H2dRegisterFis::CreateRead(sector);
  // create the command table
  cmdtable->SetCommandFis(readFis);
  cmdtable->SetPrdtEntry(0,
      PrdtEntry{reinterpret_cast<void*>(ibuffer), true, 511}
  );

  // set the new command header
  auto cmdheader = CommandHeader{cmdtable,
    1,
    0b0000'0'100,
    0b010'00101,
  };

  // now perform the logic to read / write
  int freeSlot = Port(port).EmptyCommandSlot();
  auto cmdlist = Port(port).CommandListPtr();
  cmdlist->m_headers[freeSlot] = cmdheader;
  Port(port).IssueCommand(freeSlot);

  // Now we need to wait for the command to finish
  while(Port(port).CommandSlotSet(freeSlot)){
    // spin lol
    ;;
  }
}

void AhciDriver::WriteSectorPolled(
  std::uint8_t port, 
  std::uint64_t sector, 
  std::uint8_t* buffer
){
  WriteSector(port, sector, buffer);
  WaitForPortInterrupt(port);
}

void AhciDriver::WriteSectorPolled(
  std::uint8_t port, 
  std::uint64_t sector, 
  std::uint8_t* buffer,
  CommandTable* cmdtable
){
  WriteSector(port, sector, buffer, cmdtable);
  WaitForPortInterrupt(port);
}

void AhciDriver::ReadSectorPolled(
  std::uint8_t port, 
  std::uint64_t sector, 
  std::uint8_t* buffer
){
  ReadSector(port, sector, buffer);
  WaitForPortInterrupt(port);
}

void AhciDriver::ReadSectorPolled(
  std::uint8_t port, 
  std::uint64_t sector, 
  std::uint8_t* buffer,
  CommandTable* cmdtable
){
  ReadSector(port, sector, buffer, cmdtable);
  WaitForPortInterrupt(port);
}

}
