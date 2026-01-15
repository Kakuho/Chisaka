#pragma once

#include "kassert.hpp"

#include "file_descriptor_manager.hpp"
#include "data_block.hpp"
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
  static constexpr std::uint64_t INODE_BASE          = 0x01;
  static constexpr std::uint64_t ALLOC_SIZE          = 1024;

  public:
    MfsManager(AhciDriverType& ahcidriver, std::uint8_t disk);
    FileDescriptor* Open();
    void Close(FileDescriptor fd);
    void ChDir();

    Drivers::Ahci::AhciDisk& Disk(){ return m_disk;}

    std::uint64_t InodeBlockAddress(std::size_t index);
    std::uint64_t DataBlockAddress(std::size_t index);

    std::uint64_t NextFreeInode();
    std::uint64_t NextFreeDataBlock();

  private:
    void Initialise();
    void InitialiseSuperBlock();
    void InitialiseSuperBlock(std::uint64_t maxInodes, std::uint64_t maxDataBlocks);
    void FlushSuperBlock();

    void InitialiseKernelBuffers();

    std::uint64_t InitialiseInodes(std::size_t maxInodes);
    void InitialiseInodes();
    void InitialiseFreeInode(DiskInode& inode, std::uint64_t nextInode);

    void InitialiseDataBlocks(std::size_t maxDataBlocks);
    void InitialiseDataBlocks();
    void InitialiseFreeDataBlock( DataBlock* dblock,
        std::size_t blockNumber, std::size_t maxDBlocks);

    void AllocateInode();
    void AllocateDataBlock();       

  private:
    // file system management fields
    Inode* currentdir;
    Inode* rootdir; 
    std::uint64_t m_superBlockBase;
    std::uint64_t m_inodeBase;
    std::uint64_t m_dataBlockBase;
    // kernel data structures
    Drivers::Ahci::AhciDisk& m_disk;
    Drivers::Ahci::CommandTable* m_cmdTable;
    SuperBlock* m_sbBuffer;
    DataBlock* m_dbBuffer;
    InodeBlock* m_inodeBuffer;
};

}
