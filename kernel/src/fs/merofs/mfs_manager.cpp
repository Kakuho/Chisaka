#include "mfs_manager.hpp"
#include "drivers/ahci/class_interface/ahci_driver.hpp"
#include "memory/heap/allocator.hpp"
#include <concepts>
#include <cstddef>

namespace Fs::Merofs{

MfsManager::MfsManager(AhciDriverType& ahcidriver, std::uint8_t disk)
  :
    currentdir{nullptr},
    rootdir{nullptr},
    m_superBlockBase{0},
    m_inodeBase{0},
    m_dataBlockBase{0},
    m_disk{ahcidriver.Disk(disk)},
    m_cmdTable{nullptr},
    m_sbBuffer{nullptr},
    m_dbBuffer{nullptr},
    m_inodeBuffer{nullptr}
{
  kout << "Initialising Mero File System..." << '\n';
  InitialiseKernelBuffers();
  InitialiseSuperBlock(65534, 1000);
  auto lastInodeSector = InitialiseInodes(m_sbBuffer->totalInodes);
  m_dataBlockBase = lastInodeSector + 1;
  kout << "datablocks base address: " << m_dataBlockBase << '\n';
  InitialiseDataBlocks(m_sbBuffer->totalDataBlocks);
}

void MfsManager::InitialiseSuperBlock(
  std::uint64_t maxInodes, 
  std::uint64_t maxDataBlocks
){
  kout << "Initialising super block data fields...\n";
  std::size_t totalInodeBytes = maxInodes * sizeof(DiskInode);
  std::size_t inodeSectors = totalInodeBytes / Disk().SectorSize();
  m_sbBuffer->totalInodeBlocks = inodeSectors;
  m_sbBuffer->totalInodes = maxInodes;
  m_sbBuffer->freeInodes = maxInodes;
  m_sbBuffer->usedInodes = 0;

  m_sbBuffer->totalDataBlocks = maxDataBlocks;
  m_sbBuffer->freeDataBlocks = maxDataBlocks;
  m_sbBuffer->usedDataBlocks = 0;

  m_sbBuffer->freeInodeHead = 0;
  m_sbBuffer->freeDataBlockHead = 0;

  m_sbBuffer->totalPhysBlocks = m_sbBuffer->totalDataBlocks + m_sbBuffer->totalInodeBlocks;

  m_superBlockBase = 0;
  FlushSuperBlock();
}

void MfsManager::FlushSuperBlock()
{
  Disk().WritePolled(0, reinterpret_cast<std::uint8_t*>(m_sbBuffer));
}

void MfsManager::InitialiseKernelBuffers(){
  m_cmdTable = Mem::Heap::Allocator::New<Drivers::Ahci::CommandTable>();
  m_sbBuffer = Mem::Heap::Allocator::New<SuperBlock>();
  m_dbBuffer = Mem::Heap::Allocator::New<DataBlock>();
  m_inodeBuffer = Mem::Heap::Allocator::New<InodeBlock>();
}

std::uint64_t MfsManager::InitialiseInodes(std::size_t maxInodes){
  // returns the address of the last sector used
  kout << "Initialising Inode Blocks..." << '\n';
  m_inodeBase = m_superBlockBase + 1;
  kassert(m_inodeBase == 1);
  std::size_t totalInodeBytes = maxInodes * sizeof(DiskInode);
  std::size_t inodeSectors = totalInodeBytes / Disk().SectorSize();
  kout << "Total InodeSectors: " << inodeSectors << '\n';
  std::uint64_t currentInode = 0;
  for(std::size_t currSector = 0; currSector < inodeSectors; currSector++){
    //kout << "Initialising Sector " << m_inodeBase + currSector << '\n';
    Aii::Memset(reinterpret_cast<void*>(m_inodeBuffer), 0xBB, 512);
    for(std::size_t j = 0; j < InodeBlock::INODES; j++){
      InitialiseFreeInode(m_inodeBuffer->diskinodes[j], currentInode +1);
      currentInode++;
    }
    Drivers::Ahci::AhciDriver::Get().WriteSector(
        0, 
        m_inodeBase + currSector, 
        reinterpret_cast<std::uint8_t*>(m_inodeBuffer), 
        m_cmdTable
    );
    Drivers::Ahci::AhciDriver::Get().WaitForPortInterrupt(0);
  }
  Mem::Heap::Allocator::Delete(m_inodeBuffer);
  return inodeSectors;
}

void MfsManager::InitialiseFreeInode(DiskInode& inode, std::uint64_t nextInode){
  inode.type = DiskInode::Type::File;
  for(auto i = 0ul;  i < inode.data.blockAddr.direct.Size(); i++){
    std::uint64_t initval = 0xfffffffffffffff0 + i;
    inode.data.blockAddr.direct[i] = initval;
  }
  inode.data.blockAddr.indirect = 0xaaaaaaaaaaaaaaaa;
  inode.data.blockAddr.doubleIndirect = 0xeeeeeeeeeeeeeeee;
  inode.nextFreeInode = nextInode;
  for(auto i = 0ul;  i < inode.rsv.Size(); i++){
    inode.rsv[i] = 0x99;
  }
}

void MfsManager::InitialiseDataBlocks(std::size_t maxDataBlocks){
  // data blocks are either a buffer of 512 bytes or a linked list member
  // they compose the entire physical medium's sector
  kout << "Initialising Data Blocks... \n";
  for(std::size_t currSector = 0; currSector < maxDataBlocks; currSector++){
    std::uint64_t address = m_dataBlockBase + currSector;
    //kout << "Initialising Sector " << address << '\n';
    InitialiseFreeDataBlock(m_dbBuffer, currSector, maxDataBlocks);
    Drivers::Ahci::AhciDriver::Get().WriteSector(
        0, 
        address, 
        reinterpret_cast<std::uint8_t*>(m_dbBuffer), 
        m_cmdTable
    );
    Drivers::Ahci::AhciDriver::Get().WaitForPortInterrupt(0);
  }
  std::uint64_t lastDataBlock = m_dataBlockBase + maxDataBlocks-1;
  kout << "Last DataBlock: " << lastDataBlock << '\n';
}

void MfsManager::InitialiseFreeDataBlock(
    DataBlock* dblock,
    std::size_t blockNumber, 
    std::size_t maxDBlocks
){
  Aii::Memset(reinterpret_cast<void*>(dblock), 0xBB, 512);
  dblock->used = false;
  if(blockNumber == 0){
    dblock->links.prev = 0;
  }
  else{
    dblock->links.prev = blockNumber - 1;
  }
  if(blockNumber == maxDBlocks-1){
    dblock->links.next = 0;
  }
  else{
    dblock->links.next = blockNumber + 1;
  }
}

std::uint64_t MfsManager::NextFreeInode(){
  Drivers::Ahci::AhciDriver::Get().ReadSectorPolled(
    0, 
    SUPER_BLOCK_ADDRESS, 
    reinterpret_cast<std::uint8_t*>(m_sbBuffer), 
    m_cmdTable
  );
  return m_sbBuffer->freeInodeHead;
}

std::uint64_t MfsManager::NextFreeDataBlock(){
  Drivers::Ahci::AhciDriver::Get().ReadSectorPolled(
    0, 
    SUPER_BLOCK_ADDRESS, 
    reinterpret_cast<std::uint8_t*>(m_sbBuffer), 
    m_cmdTable
  );
  return m_sbBuffer->freeDataBlockHead;
}

std::uint64_t MfsManager::InodeBlockAddress(std::size_t index){
  std::uint64_t blockNum = index / 7;
  if(blockNum > m_sbBuffer->totalInodeBlocks){
    return -1;
  }
  else{
    return blockNum + m_inodeBase;
  }
}

std::uint64_t MfsManager::DataBlockAddress(std::size_t index){
  if(index > m_sbBuffer->totalDataBlocks){
    return -1;
  }
  else{
    return index + m_dataBlockBase;
  }
}

void MfsManager::AllocateInode(){
  auto nextInode = NextFreeInode();
  std::uint64_t blockAddress = InodeBlockAddress(nextInode);
  std::uint8_t offset = nextInode % 7;
  Drivers::Ahci::AhciDriver::Get().ReadSectorPolled(
    0, 
    blockAddress, 
    reinterpret_cast<std::uint8_t*>(m_inodeBuffer), 
    m_cmdTable
  );
  auto diskinodes = m_inodeBuffer->diskinodes;
  DiskInode dinode = diskinodes[offset];
  // not sure how to proceed after this
}

void MfsManager::AllocateDataBlock(){

}


}
