#pragma once

// Organisation:
//  The file system is split up into 3 partitions.
//
//  The second partition of MeroFs is called the inode partition and contains the inodes
//
//  Inodes contains the metadata of files, and the data composing files are contained within physical data blocks, 
//  whose addresses are pointed to by the blockAddr member of the inode
//
//  A InodeBlock is a single sector of the physical media, containing a array of on disk inodes. The inode partition
//  is thus composed of multiple InodeBlocks

#include <cstdint>

#include "aii/array.hpp"

namespace Fs::Merofs{
  struct DiskInode{
    struct AddressStruct{
      public:
        std::uint64_t& operator[](std::uint8_t index){
          if(index < 4){
            return direct[index];
          }
          else if(index == 4){
            return indirect;
          }
          else{
            return doubleIndirect;
          }
        }
      public:
        std::uint64_t direct[4];
        std::uint64_t indirect;
        std::uint64_t doubleIndirect;
    };
    static_assert(sizeof(AddressStruct) <= 48);

    enum class Type{Free, File, Directory};

    struct Data{
      AddressStruct blockAddr;
    };

    public:
    Type type;
    union{
      std::uint64_t nextFreeInode;
      Data data;
    };
  };
  static_assert(sizeof(DiskInode) < 64);

  struct InodeBlock{
    static constexpr std::size_t INODES = 9;
    Aii::Array<DiskInode, INODES> diskinodes;
  };
  static_assert(sizeof(InodeBlock) < 512);
}
