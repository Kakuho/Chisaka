#pragma once

#include <cstdint>

#include "process.hpp"
#include "kmalloc.hpp"

#include "x86_64/paging/upper_page_table.hpp"

namespace Proc{

class ProcessManager{
  public:
    ProcessManager() = default;
    ~ProcessManager() = default;
    void Init();
    ProcessManager& Get();

    Process* SpawnProcess();
    pid_t Kill(Process* process);

    constexpr Process* Current(){return currentProcess;}
    void Switch(Process* process);

    Process* Find(pid_t pid);
    
    void Swapout(Process* process, std::uint64_t linearAddress);

  private:
    void CreatePagetable(Process* process);
    void MapKernel(X8664::UpperPageTable* ptbl);

  private:
    Process* root;
    Process* currentProcess;
    std::int16_t procCount = 0;
};

}
