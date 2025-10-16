#pragma once

#include "file_descriptor_manager.hpp"
#include "disk_inode.hpp"
#include "superblock.hpp"
#include "inode.hpp"

#include "drivers/ahci/class_interface/ahci_driver.hpp"
#include "drivers/ahci/class_interface/ahci_disk.hpp"
#include "drivers/ahci/structs/command_table.hpp"
#include "drivers/serial/kostream.hpp"
#include "aii/string.h"

#include "memory/heap/allocator.hpp"

namespace Fs::Merofs{

struct MfsManager{
  // the main interface to the mero file system
  using AhciDriverType = Drivers::Ahci::AhciDriver;
  static constexpr std::uint64_t SUPER_BLOCK_ADDRESS = 0x00;
  static constexpr std::size_t ALLOC_SIZE = 1024;

  public:
    MfsManager(AhciDriverType& ahcidriver, std::uint8_t disk);
    FileDescriptor* Open();
    void Close(FileDescriptor fd);
    void ChDir();

    Drivers::Ahci::AhciDisk& Disk(){ return m_disk;}

  private:
    void Initialise();
    void InitialiseSuperBlock();
    void InitialiseInodes(std::size_t maxInodes);
    void InitialiseDataBlocks();

    void AllocateDataBlock();       
    void AllocateInode();

  private:
    Drivers::Ahci::AhciDisk& m_disk;
    SuperBlock* m_superBlock;
    Inode* currentdir;
    Inode* rootdir; 
    std::uint64_t m_superBlockBase;
    std::uint64_t m_inodeBase;
    std::uint64_t m_dataBlockBase;
};

}
