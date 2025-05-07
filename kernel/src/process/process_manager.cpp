#include "process_manager.hpp"
#include "x86_64/paging/upper_page_table.hpp"

namespace{
  Proc::ProcessManager manager;
}

namespace Proc{

void ProcessManager::Init(){
  root = reinterpret_cast<Process*>(Kernel::kmalloc(sizeof(Process)));
  root->child = nullptr;
  root->childId = -1;
  CreatePagetable(root);
}

Process* ProcessManager::SpawnProcess(){
  auto spawned = reinterpret_cast<Process*>(Kernel::kmalloc(sizeof(Process)));
  spawned->id = procCount++;
  spawned->SetParent(currentProcess);
  if(spawned->parent->child){
    spawned->parent->child->InsertSibling(spawned);
  }
  else{
    spawned->parent->child = spawned;
  }
  CreatePagetable(spawned);
  return spawned;
}

void ProcessManager::CreatePagetable(Process* process){
  auto baseTable = reinterpret_cast<X8664::UpperPageTable*>(
      Kernel::kmalloc(sizeof(X8664::UpperPageTable))
  );
  MapKernel(baseTable);
}

void ProcessManager::MapKernel(X8664::UpperPageTable* ptbl){
  // Kernel is mapped from 0xffff8000'00000000 -> 0xffff8002'40000000 
  // and is an identity mapping of the first 0x2'40000000 bytes of ram
  // ptbl->Map(0xffff8000'00000000, kernelTbl);
}

Process* ProcessManager::Find(pid_t pid){

}

}
