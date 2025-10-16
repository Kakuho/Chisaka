#include "mfs_manager.hpp"
#include "drivers/ahci/class_interface/ahci_driver.hpp"
#include "memory/heap/allocator.hpp"
#include <cstddef>

namespace Fs::Merofs{

MfsManager::MfsManager(AhciDriverType& ahcidriver, std::uint8_t disk)
  :
    m_disk{ahcidriver.Disk(disk)},
    currentdir{nullptr},
    rootdir{nullptr}
{
  kout << "Initialising Mero File System..." << '\n';
  InitialiseSuperBlock();
  InitialiseInodes(65534);
}

void MfsManager::InitialiseSuperBlock(){
  kout << "writting super block to the filesystem...";
  auto* superblock = Mem::Heap::Allocator::New<SuperBlock>();
  Disk().WritePolled(0, reinterpret_cast<std::uint8_t*>(superblock));
  m_superBlockBase = 0;
}

void MfsManager::InitialiseInodes(std::size_t maxInodes){
  kout << "Initialising Inode Blocks..." << '\n';
  m_inodeBase = m_superBlockBase + 1;
  kassert(m_inodeBase == 1);
  std::size_t totalInodeBytes = maxInodes * sizeof(DiskInode);
  std::size_t inodeSectors = totalInodeBytes / Disk().SectorSize();
  kout << "Total InodeSectors: " << inodeSectors << '\n';
  m_dataBlockBase = inodeSectors + 1;
  std::uint64_t currentInode = 0;
  auto* inodeBuffer = Mem::Heap::Allocator::New<InodeBlock>();
  auto* cmdTable = Mem::Heap::Allocator::New<Drivers::Ahci::CommandTable>();
  for(std::size_t currSector = 0; currSector < inodeSectors; currSector++){
    //if(currSector == 0x590) break;
    Aii::Memset(reinterpret_cast<void*>(inodeBuffer), 0, 512);
    for(std::size_t j = 0; j < InodeBlock::INODES; j++){
      inodeBuffer->diskinodes[j].nextFreeInode = currentInode+1;
    }
    Drivers::Ahci::AhciDriver::Get().WriteSector(0, 
        m_inodeBase + currSector, 
        reinterpret_cast<std::uint8_t*>(inodeBuffer), 
        cmdTable
    );
    Drivers::Ahci::AhciDriver::Get().WaitForPortInterrupt(0);
  }
}

void MfsManager::Initialise(){
  // initialise the super block
  // initialise the inodes
  // initialise the data blocks
}

}
