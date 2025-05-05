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

namespace Proc{

enum class Status{
  Running,
  Stopped,
  ExitDead
};

struct UserExecution;

struct Process{
  // Todo:
  //  * Provide a Page Table

  static std::uint8_t pid_count;

  Process();
  ~Process();                 // destruct all
                              // associated resources
  void SetupSwapp();
                              
  void SpawnChildren();
  void MoveChildren();

  std::uint8_t m_id;
  std::uint8_t m_parentid;
  Status m_stat;
  Process* m_sibling;
  UserExecution* m_user;
};

struct UserExecution{
  Process* m_process;
  X8664Pcb pcb;
};

} // namespace Proc
