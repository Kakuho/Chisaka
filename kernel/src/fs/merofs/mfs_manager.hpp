#pragma once

#include "file_descriptor_manager.hpp"
#include "inode.hpp"

namespace Fs::Merofs{

struct MfsManager{
  // the main interface to the mero file system
  public:
    FileDescriptor* Open();
    void Chdir();

  private:
    Inode* currentdir;
    Inode* rootdir; 
};

}
