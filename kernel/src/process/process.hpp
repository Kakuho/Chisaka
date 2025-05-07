#pragma once

//  Basic Abstraction of Process
//
//    * Process       - The required kernel state of the process
//    * UserExecution - The user space information of the process.
//                      
//  UserExecution can be swapped out to disk, however Process must remain within 
//  ram at all times

#include <cstdint>

#include "x8664pcb.hpp"
#include "x86_64/paging/upper_page_table.hpp"

namespace Proc{

enum class Status{
  Running,
  Stopped,
  ExitDead
};

using pid_t = std::uint8_t;

struct UserExecution;

struct Process{
  public:
    void SetParent(Process* proc);
    void InsertSibling(Process* proc);
    void InsertChild(Process* proc);

  public:
    pid_t id;
    pid_t parentId;
    pid_t childId;
    Status m_stat;
    Process* parent;
    Process* sibling;
    Process* child;
    X8664::UpperPageTable* pageTable;
    UserExecution* m_user;
};

struct UserExecution{
  Process* m_process;
  X8664Pcb pcb;
};

} // namespace Proc
