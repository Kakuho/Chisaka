#include <cstdint>

#include "memory/heap/allocator.hpp"
#include "aii/array.hpp"

namespace Fs::Merofs{

struct FileDescriptor{
  // immutable
  // file descriptors starts at 1, 0 is the null value
  static constexpr std::uint8_t NULLFD= 0;
  public:
    FileDescriptor(): m_fd{NULLFD}{}
    FileDescriptor(std::uint8_t fd): m_fd{fd}{}

    bool IsNull() const{ return m_fd == NULLFD;}

    std::uint8_t Get() const{ return m_fd;}
    void Reset(){ m_fd = NULLFD;}

  private:
    std::uint8_t m_fd;
};

struct FileDescriptorTable{
  struct TableChunk{
    // uses a linked list of table entries
      static constexpr std::uint8_t entries = 32;
    public:
      TableChunk();
      ~TableChunk();

      bool Full() const{ return used == entries;}
      std::uint8_t NextFree() const;

      FileDescriptor& Entry(std::uint8_t index){
        return table[index];
      }

      const FileDescriptor& Entry(std::uint8_t index) const{
        return table[index];
      }

      FileDescriptor& operator[](std::uint8_t index){ return Entry(index); }
      const FileDescriptor& operator[](std::uint8_t index) const{ return table[index];}

    public:
      TableChunk* prev;
      TableChunk* next;
      std::uint8_t used;
      Aii::Array<FileDescriptor, entries> table;
  };

  public:
    FileDescriptorTable();
    ~FileDescriptorTable();
    bool Full() const{ return m_full;}
    std::size_t MaxEntries() const{ return m_maxEntries;}

    const FileDescriptor* Get(std::uint8_t fd) const;
    FileDescriptor* Allocate();
    void Remove(std::uint8_t fd);

    constexpr std::uint8_t TableEntries() const{ return TableChunk::entries;}
    std::size_t NumberOfChunks() const;

  private:
    TableChunk* AllocateChunk();

  private:
    TableChunk* m_root;
    std::size_t m_maxEntries;
    bool m_full;
};

struct FileDescriptorManager{
  public:
    FileDescriptorManager() = default;

    const FileDescriptor* GetFd(std::uint8_t fd) const{ return fdtable.Get(fd); }
    FileDescriptor* CreateNewFd(){ return fdtable.Allocate(); }
    void Remove(std::uint8_t fd){fdtable.Remove(fd); }

  private:
    FileDescriptorTable fdtable;
};

}
