#include <cstdint>

#include "inode.hpp"

namespace Fs::Merofs{

struct FileSession{
  public:
    // represents an opened file session
    FileSession() = default;

  public:
    std::uint8_t flags;
    std::uint8_t type;
    std::uint8_t refcount;
    Inode* inodeptr;
    std::size_t offset;
};

struct FileSessionTable{
  FileSessionTable() = default;

};

}
