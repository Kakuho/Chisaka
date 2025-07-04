#pragma once

// The process manager is the main way to interact with processes within the system

#include <cstdint>

#include "process.hpp"
#include "kmalloc.hpp"

#include "x86_64/paging/upper_page_table.hpp"
#include "x86_64/paging/upper_page_entry.hpp"
#include "x86_64/paging/utilities.hpp"

namespace Proc{

class ProcessManager{
  public:
    ProcessManager() = default;
    ~ProcessManager();

    void Init();
    ProcessManager& Get();

    Process* CreateProcess();
    pid_t Kill(Process* process);
    pid_t Kill(pid_t pid);

    constexpr Process* Current(){return m_currentProcess;}
    void Switch(Process* process);

    Process* Find(pid_t pid);
    
    void Swapout(Process* process, std::uint64_t linearAddress);

  private:
    void InitRootProcess();
    void CreatePagetable(Process* process);
    void MapKernel(X8664::UpperPageTable* ptbl);

  private:
    Process* m_root;              // the root process has the kernel table
    Process* m_currentProcess;
    X8664::UpperPageTable* m_kernelTable;
    std::int16_t procCount = 0;
};

}
